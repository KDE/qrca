/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *  SPDX-FileCopyrightText: 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *  SPDX-FileCopyrightText: 2020 Nicolas Fella <nicolas.fella@gmx.de>
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
#include <NetworkManagerQt/WirelessSetting>
#include <NetworkManagerQt/WirelessSecuritySetting>
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
    auto p = Prison::MeCard::parse(wifiSetting);;
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
    auto p = Prison::MeCard::parse(wifiCode);
    if (!p) {
        return;
    }

#if HAVE_NETWORKMANAGER
    using namespace NetworkManager;
    auto settings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wireless));
    settings->setUuid(ConnectionSettings::createNewUuid());
    settings->setAutoconnect(true);
    settings->fromMeCard((*p).toVariantMap());
    NetworkManager::addConnection(settings->toMap());
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
