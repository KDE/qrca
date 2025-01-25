/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *  SPDX-FileCopyrightText: 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
 *  SPDX-FileCopyrightText: 2025 Kai Uwe Broulik <kde@broulik.de>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config-qrca.h"
#include <QtGlobal>

#ifndef Q_OS_ANDROID
#include <KService>
#include <KIO/ApplicationLauncherJob>
#endif

#if HAVE_NETWORKMANAGER
#include <NetworkManagerQt/Manager>
#include <NetworkManagerQt/Security8021xSetting>
#include <NetworkManagerQt/Settings>
#include <NetworkManagerQt/WirelessDevice>
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>

#include <QDBusPendingCallWatcher>
#endif

#include <QClipboard>
#include <QCryptographicHash>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QGuiApplication>
#include <QImage>
#include <QMimeData>
#include <QStandardPaths>
#include <QTemporaryFile>

#include <KContacts/Addressee>
#include <KContacts/VCardConverter>
#include <KLocalizedString>

#include <Prison/MeCard>

#include "Qrca.h"
#include "QrCodeContent.h"

Qrca::Qrca() = default;

QString Qrca::encodeText() const noexcept
{
    return m_encodeText;
}

void Qrca::setEncodeText(const QString &encodeText) noexcept
{
    m_encodeText = encodeText;

    Q_EMIT encodeTextChanged();
}

bool Qrca::wifiMode() const noexcept
{
    return m_wifiMode;
}

void Qrca::setWifiMode(bool wifiMode) noexcept
{
    if (m_wifiMode != wifiMode) {
        m_wifiMode = wifiMode;
        Q_EMIT wifiModeChanged();
    }
}

bool Qrca::connectingToWifi() const noexcept
{
    return m_connectingToWifi;
}

void Qrca::setConnectingToWifi(bool connectingToWifi) noexcept
{
    if (m_connectingToWifi != connectingToWifi) {
        m_connectingToWifi = connectingToWifi;
        Q_EMIT connectingToWifiChanged(connectingToWifi);
    }
}

void Qrca::saveVCard(const QString &text) noexcept
{
    QString path = QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation) + QStringLiteral("/kpeoplevcard");

    QCryptographicHash hash(QCryptographicHash::Sha1);
    hash.addData(getVCardName(text).toUtf8());

    QFile file(path + QStringLiteral("/") + QString::fromLatin1(hash.result().toHex()) + QStringLiteral(".vcf"));

    if (!file.open(QFile::WriteOnly)) {
        qWarning() << "Couldn't save vCard: Couldn't open file for writing.";
        return;
    }
    file.write(text.toUtf8(), text.toUtf8().length());
    file.close();
}

QString Qrca::getVCardName(const QString &text) noexcept
{
    KContacts::VCardConverter converter;
    KContacts::Addressee adressee = converter.parseVCard(text.toUtf8());

    return adressee.realName();
}

QString Qrca::newQrCodeSaveLocation() noexcept
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    const QString path = directory + QDir::separator() + QDateTime::currentDateTime().toString(Qt::ISODate) + QStringLiteral(".png");
    return path;
}

void Qrca::copyToClipboard(const QrCodeContent &content) noexcept
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (!content.isPlainText()) {
        auto md = new QMimeData;
        md->setData(QStringLiteral("application/octet-stream"), content.binaryContent());
        clipboard->setMimeData(md);
    } else {
        clipboard->setText(content.text());
    }
}

void Qrca::copyImageToClipboard(const QImage &image) {
    QClipboard *clipboard = QGuiApplication::clipboard();
    clipboard->setImage(image);
}

bool Qrca::hasApplication(const QString &appId) const
{
#ifndef Q_OS_ANDROID
    return KService::serviceByDesktopName(appId);
#else
    return false;
#endif
}

QString Qrca::applicationIconName(const QString &appId) const
{
#ifndef Q_OS_ANDROID
    const auto service = KService::serviceByDesktopName(appId);
    return service ? service->icon() : QString();
#else
    return {};
#endif
}

void Qrca::openInApplication(const QrCodeContent &content, const QString &appId)
{
#ifndef Q_OS_ANDROID
    QTemporaryFile file;
    if (!file.open()) {
        qWarning() << file.errorString();
        return;
    }
    if (content.isPlainText()) {
        file.write(content.text().toUtf8());
    } else {
        file.write(content.binaryContent());
    }
    file.flush();
    file.setAutoRemove(false);

    auto job = new KIO::ApplicationLauncherJob(KService::serviceByDesktopName(appId), this);
    job->setUrls({QUrl::fromLocalFile(file.fileName())});
    job->setRunFlags(KIO::ApplicationLauncherJob::DeleteTemporaryFiles);
    job->start();
#endif
}

QString Qrca::wifiName(const QString& wifiSetting) const
{
    auto p = Prison::MeCard::parse(wifiSetting);
    return p ? p->value(u"S") : QString();
}

bool Qrca::canConnectToWifi() const
{
#if HAVE_NETWORKMANAGER
    return NetworkManager::isNetworkingEnabled();
#else
    return false;
#endif
}

// see https://github.com/zxing/zxing/blob/master/android/src/com/google/zxing/client/android/wifi/WifiConfigManager.java
// for the details not found in the documentation
#if HAVE_NETWORKMANAGER
static struct {
    const char *name;
    NetworkManager::Security8021xSetting::EapMethod method;
} constexpr const eap_methods[] = {
    { "PEAP", NetworkManager::Security8021xSetting::EapMethodPeap },
    { "PWD", NetworkManager::Security8021xSetting::EapMethodPwd },
    { "TLS", NetworkManager::Security8021xSetting::EapMethodTls },
    { "TTLS", NetworkManager::Security8021xSetting::EapMethodTtls },
};

static struct {
    const char *name;
    NetworkManager::Security8021xSetting::AuthMethod method;
} constexpr const auth_methods[] = {
    { "GTC", NetworkManager::Security8021xSetting::AuthMethodGtc },
    { "MSCHAP", NetworkManager::Security8021xSetting::AuthMethodMschap },
    { "MSCHAPV2", NetworkManager::Security8021xSetting::AuthMethodMschapv2 },
    { "PAP", NetworkManager::Security8021xSetting::AuthMethodPap },
};
#endif

void Qrca::connectToWifi(const QString &wifiCode)
{
#if HAVE_NETWORKMANAGER
    auto p = Prison::MeCard::parse(wifiCode);
    if (!p) {
        return;
    }

    using namespace NetworkManager;

    const QByteArray ssid = p->value(u"S").toLatin1();

    // Find an existing configuration to update if hotspot isn't visible or in reach.
    Connection::Ptr existingConnection;

    auto settings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wireless));
    settings->fromMeCard(p->toVariantMap());

    const auto connections = NetworkManager::listConnections();
    for (const auto &connection : connections) {
        if (connection->name().isEmpty() || connection->uuid().isEmpty()) {
            continue;
        }

        ConnectionSettings::Ptr settings = connection->settings();
        if (settings->connectionType() != ConnectionSettings::Wireless) {
            continue;
        }

        auto wirelessSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
        if (wirelessSetting->ssid() != ssid) {
            continue;
        }

        existingConnection = connection;
        break;
    }

    // TODO Pick the wifi device that has the corresponding access point.
    WirelessDevice::Ptr wifiDevice;

    const auto networkInterfaces = NetworkManager::networkInterfaces();
    for (const auto &device : networkInterfaces) {
        if (device->type() == Device::Wifi) {
            wifiDevice = device.objectCast<WirelessDevice>();
            break;
        }
    }

    if (!wifiDevice) {
        qWarning() << "Failed to find wifi device for MeCard";
        Q_EMIT wifiConnectionFailed(i18n("Failed to find wifi device"));
        return;
    }

    setConnectingToWifi(true);

    if (existingConnection) {
        qDebug() << "Updating existing connection" << existingConnection->uuid() << existingConnection->path() << "from MeCard";

        // Override default null UUID with existing one.
        settings->setUuid(existingConnection->uuid());

        const QString connectionPath = existingConnection->path();
        const QString deviceUni = wifiDevice->uni();

        auto call = existingConnection->update(settings->toMap());
        auto *watcher = new QDBusPendingCallWatcher(call, this);
        connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher, connectionPath, deviceUni] {
            watcher->deleteLater();

            QDBusPendingReply<> reply = *watcher;
            if (reply.isError()) {
                qWarning() << "Failed to update existing connection" << reply.error().message();
                Q_EMIT wifiConnectionFailed(reply.error().message());
                setConnectingToWifi(false);
                return;
            }

            auto call = NetworkManager::activateConnection(connectionPath, deviceUni, QString());
            auto *watcher2 = new QDBusPendingCallWatcher(call, this);
            connect(watcher2, &QDBusPendingCallWatcher::finished, this, [this, watcher2] {
                watcher2->deleteLater();

                QDBusPendingReply<QDBusObjectPath> reply = *watcher2;
                if (reply.isError()) {
                    qWarning() << "Failed to activate connection" << reply.error().message();
                    Q_EMIT wifiConnectionFailed(reply.error().message());
                } else {
                    Q_EMIT wifiConnected();
                }
                setConnectingToWifi(false);
            });
        });
        return;
    }

    // Add new connection and activate it.
    settings->setUuid(ConnectionSettings::createNewUuid());
    settings->setAutoconnect(true);
    // TODO what about UiUtils::setConnectionDefaultPermissions that plasma-nm does?

    auto call = NetworkManager::addAndActivateConnection(settings->toMap(), wifiDevice->uni(), QString());
    auto *watcher = new QDBusPendingCallWatcher(call, this);
    connect(watcher, &QDBusPendingCallWatcher::finished, this, [this, watcher] {
        watcher->deleteLater();

        QDBusPendingReply<QDBusObjectPath, QDBusObjectPath> reply = *watcher;
        if (reply.isError()) {
            qWarning() << "Failed to add and activate connection" << reply.error().message();
            Q_EMIT wifiConnectionFailed(reply.error().message());
        } else {
            Q_EMIT wifiConnected();
        }
        setConnectingToWifi(false);
    });
#else
    Q_UNUSED(wifiCode)
#endif
}

QrCodeContent Qrca::resultContent(const Prison::ScanResult &result)
{
    if (result.hasBinaryData()) {
        return QrCodeContent(result.binaryData(), result.format());
    } else {
        return QrCodeContent(result.text(), result.format());
    }
}

#include "moc_Qrca.cpp"
