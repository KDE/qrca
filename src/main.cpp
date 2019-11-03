#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QCommandLineParser>

#include <KAboutData>
#include <KLocalizedString>

#include "QrCodeScannerFilter.h"
#include "Qrca.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KDE");
    QCoreApplication::setOrganizationDomain("kde.org");
    QCoreApplication::setApplicationName("qrca");

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(QCommandLineOption("encode", "Text to encode into a QR-Code", "encode", ""));
    parser.process(app);

    // setup translation string domain for the i18n calls
    KLocalizedString::setApplicationDomain("qrca");
    // create a KAboutData object to use for setting the application metadata
    KAboutData aboutData("qrca", i18n("Qrca"), "0.1",
                         i18n("A simple QR scanner"),
                         KAboutLicense::GPL_V3,
                         i18n("Copyright 2016-2019 Qrca and Kaidan authors"), QString(),
                         "https://www.plasma-mobile.org");
    // overwrite default-generated values of organizationDomain & desktopFileName
    aboutData.addAuthor(i18n("Jonah Brüchert"), QString(), QStringLiteral("jbb@kaidan.im"), QStringLiteral("jbbgameich.github.io"));
    aboutData.addAuthor(i18n("Simon Schmeißer"));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName("org.kde.qrca");
    // set the application metadata
    KAboutData::setApplicationData(aboutData);
    // in GUI apps set the window icon manually, not covered by KAboutData
    // needed for environments where the icon name is not extracted from
    // the information in the application's desktop file
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("qrca")));
    aboutData.setProgramLogo(app.windowIcon());

    qmlRegisterType<QrCodeScannerFilter>("org.kde.qrca", 1, 0, "QrCodeScannerFilter");
    qmlRegisterSingletonType<Qrca>("org.kde.qrca", 1, 0, "Qrca", [](QQmlEngine *engine, QJSEngine *scriptEngine) {
        Q_UNUSED(engine)
        Q_UNUSED(scriptEngine)

        auto *qrca = new Qrca();
        return qrca;
    });

    QQmlApplicationEngine engine;

    engine.rootContext()->setContextProperty("encodeText", parser.value("encode"));
    engine.rootContext()->setContextProperty(QStringLiteral("qrcaAboutData"), QVariant::fromValue(KAboutData::applicationData()));

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
