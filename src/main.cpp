/*
 *  SPDX-FileCopyrightText: 2019 Jonah Brüchert <jbb@kaidan.im>
 *  SPDX-FileCopyrightText: 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

#include "config-qrca.h"

#include "qrca_version.h"

#include <qglobal.h>
#ifndef Q_OS_ANDROID
#include <QApplication>
#else
#include <QGuiApplication>
#endif
#include <QCommandLineParser>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QUrl>
#include <QtQml>
#include <QQuickStyle>

#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>
#include <KLocalizedQmlContext>

#ifndef Q_OS_ANDROID
#include <KCrash>
#endif

#include "QrCodeContent.h"
// #include "QrCodeScannerFilter.h"
#include "Qrca.h"
#include "notificationmanager.h"
#include "clipboard.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#ifndef Q_OS_ANDROID
    QApplication app(argc, argv);
    // Default to org.kde.desktop style unless the user forces another style
    if (qEnvironmentVariableIsEmpty("QT_QUICK_CONTROLS_STYLE")) {
        QQuickStyle::setStyle(QStringLiteral("org.kde.desktop"));
    }
#else
    QGuiApplication app(argc, argv);
    QQuickStyle::setStyle(QStringLiteral("org.kde.breeze"));
#endif
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("qrca"));

    // setup translation string domain for the i18n calls
    KLocalizedString::setApplicationDomain("qrca");
    // create a KAboutData object to use for setting the application metadata
    KAboutData aboutData(
        QStringLiteral("qrca"), i18n("Qrca"), QStringLiteral(QRCA_VERSION_STRING), i18n("A simple QR scanner"), KAboutLicense::GPL_V3, i18n("Copyright 2016-2019 Qrca and Kaidan authors"), QString(), QStringLiteral("https://www.plasma-mobile.org"));
    // overwrite default-generated values of organizationDomain & desktopFileName
    aboutData.addAuthor(i18n("Jonah Brüchert"), QString(), QStringLiteral("jbb@kaidan.im"), QStringLiteral("https://jbbgameich.github.io"));
    aboutData.addAuthor(i18n("Simon Schmeißer"));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.qrca"));

    // set the application metadata
    KAboutData::setApplicationData(aboutData);
    // in GUI apps set the window icon manually, not covered by KAboutData
    // needed for environments where the icon name is not extracted from
    // the information in the application's desktop file
    QGuiApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("org.kde.qrca")));
    aboutData.setProgramLogo(app.windowIcon());

    QCommandLineParser parser;
    aboutData.setupCommandLine(&parser);
    parser.addOption(QCommandLineOption(QStringLiteral("encode"), QStringLiteral("Text to encode into a QR-Code"), QStringLiteral("encode"), {}));
#if HAVE_NETWORKMANAGER
    QCommandLineOption wifiOption(QStringLiteral("wifi"), i18n("Scan only for Wifi network QR-Codes"));
    parser.addOption(wifiOption);
#endif

    parser.process(app);
    aboutData.processCommandLine(&parser);

    bool wifiMode = false;
#if HAVE_NETWORKMANAGER
    wifiMode = parser.isSet(wifiOption);
#endif

    if (wifiMode) {
        app.setDesktopFileName(QStringLiteral("org.kde.qrca.wifi"));
    }

#ifndef Q_OS_ANDROID
    KCrash::initialize();
#endif

    qmlRegisterUncreatableType<QrCodeContent>("org.kde.qrca", 1, 0, "QrCodeContent", {});
    qmlRegisterSingletonType<NotificationManager>("org.kde.qrca", 1, 0, "NotificationManager", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return new NotificationManager();
    });
    qmlRegisterSingletonType<Clipboard>("org.kde.qrca", 1, 0, "Clipboard", [](QQmlEngine *engine, QJSEngine *scriptEngine) -> QObject * {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        return new Clipboard();
    });
    qRegisterMetaType<QrCodeContent>();

    Qrca qrca;
    qrca.setEncodeText(parser.value(QStringLiteral("encode")));
#if HAVE_NETWORKMANAGER
    qrca.setWifiMode(wifiMode);
#endif

    qmlRegisterSingletonInstance<Qrca>("org.kde.qrca", 1, 0, "Qrca", &qrca);

    QQmlApplicationEngine engine;
    KLocalization::setupLocalizedContext(&engine);

    engine.loadFromModule("org.kde.qrca", "Main");

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
