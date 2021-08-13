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

#include <ZXing/BarcodeFormat.h>
#include <ZXing/BitMatrix.h>
#include <ZXing/MultiFormatWriter.h>

#include "Qrca.h"
#include "QrCodeContent.h"
#include "mecardparser.h"

Qrca::Qrca() = default;

KAboutData Qrca::aboutData() const noexcept
{
    return m_aboutData;
}

void Qrca::setAboutData(const KAboutData &aboutData) noexcept
{
    m_aboutData = aboutData;

    Q_EMIT aboutDataChanged();
}

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

QImage Qrca::encode(const QString &text, const int &width) noexcept
{
    try {
        const ZXing::MultiFormatWriter writer(ZXing::BarcodeFormat::QR_CODE);

        const auto matrix = writer.encode(text.toStdWString(), width, width);

        QImage image(width, width, QImage::Format_ARGB32);

        for (int y = 0; y < matrix.height(); ++y) {
            for (int x = 0; x < matrix.width(); ++x) {
                image.setPixel(y, x, matrix.get(x, y) ? qRgb(0, 0, 0) : qRgb(255, 255, 255));
            }
        }

        return image;
    } catch (const std::invalid_argument &e) {
        passiveNotificationRequested(i18n("Generating the QR-Code failed: ") + QString::fromLatin1(e.what()));
    };

    return {};
}

QUrl Qrca::save(const QImage &image) noexcept
{
    const QString directory = QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + QStringLiteral("/qrcodes/");

    const QString path = directory + QDateTime::currentDateTime().toString(Qt::ISODate) + QStringLiteral(".png");

    const QDir dir(directory);
    if (!dir.exists()) {
        dir.mkpath(directory);
    }

    qDebug() << "Saving image to" << path;
    image.save(path, "PNG", -1);

    return QUrl::fromLocalFile(path);
}

void Qrca::copyToClipboard(const QrCodeContent &content) noexcept
{
    QClipboard *clipboard = QGuiApplication::clipboard();
    if (content.contentType() == QrCodeContent::Binary) {
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
    MeCardParser p;
    p.parse(wifiSetting);
    return p.value(u"S");
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
    MeCardParser p;
    p.parse(wifiCode);

#if HAVE_NETWORKMANAGER
    using namespace NetworkManager;
    auto settings = ConnectionSettings::Ptr(new ConnectionSettings(ConnectionSettings::Wireless));
    settings->setId(p.value(u"S"));
    settings->setUuid(ConnectionSettings::createNewUuid());
    settings->setAutoconnect(true);

    auto wifiSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wifiSetting->setInitialized(true);
    wifiSetting = settings->setting(Setting::Wireless).dynamicCast<WirelessSetting>();
    wifiSetting->setSsid(p.value(u"S").toUtf8());

    auto wifiSecurity = settings->setting(Setting::WirelessSecurity).dynamicCast<WirelessSecuritySetting>();
    const auto securityType = p.value(u"T");

    if (!securityType.isEmpty() && securityType != QLatin1String("nopass")) {
        wifiSecurity->setInitialized(true);
        wifiSetting->setSecurity(QStringLiteral("802-11-wireless-security"));
    }

    if (securityType == QLatin1String("WPA") || securityType == QLatin1String("WEP")) {
        wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaPsk);
        wifiSecurity->setPsk(p.value(u"P"));
        wifiSecurity->setPskFlags(NetworkManager::Setting::AgentOwned);
    } else if (securityType == QLatin1String("WPA2-EAP")) {
        wifiSecurity->setKeyMgmt(NetworkManager::WirelessSecuritySetting::WpaEap);
        auto sec8021x = settings->setting(Setting::Security8021x).dynamicCast<Security8021xSetting>();
        sec8021x->setAnonymousIdentity(p.value(u"A"));
        sec8021x->setIdentity(p.value(u"I"));
        sec8021x->setPassword(p.value(u"P"));

        const auto eapMethod = p.value(u"E");
        for (const auto &method : eap_methods) {
            if (eapMethod == QLatin1String(method.name)) {
                sec8021x->setEapMethods({method.method});
                break;
            }
        }
        const auto phase2AuthMethod = p.value(u"PH2");
        for (const auto &method : auth_methods) {
            if (eapMethod == QLatin1String(method.name)) {
                sec8021x->setPhase2AuthMethod(method.method);
                break;
            }
        }
    }

    NetworkManager::addConnection(settings->toMap());
#endif
}
