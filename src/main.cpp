#include <QApplication>
#include <QIcon>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>
#include <QCommandLineParser>

#include <KAboutData>
#include <KLocalizedContext>
#include <KLocalizedString>

#include "QrCodeScannerFilter.h"
#include "Qrca.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName(QStringLiteral("KDE"));
    QCoreApplication::setOrganizationDomain(QStringLiteral("kde.org"));
    QCoreApplication::setApplicationName(QStringLiteral("qrca"));

    QCommandLineParser parser;
    parser.addHelpOption();
    parser.addOption(QCommandLineOption(QStringLiteral("encode"),
                                        QStringLiteral("Text to encode into a QR-Code"),
                                        QStringLiteral("encode"),
                                        {}));
    parser.process(app);

    // setup translation string domain for the i18n calls
    KLocalizedString::setApplicationDomain("qrca");
    // create a KAboutData object to use for setting the application metadata
    KAboutData aboutData(QStringLiteral("qrca"), i18n("Qrca"), QStringLiteral("0.1"),
                         i18n("A simple QR scanner"),
                         KAboutLicense::GPL_V3,
                         i18n("Copyright 2016-2019 Qrca and Kaidan authors"), QString(),
                         QStringLiteral("https://www.plasma-mobile.org"));
    // overwrite default-generated values of organizationDomain & desktopFileName
    aboutData.addAuthor(i18n("Jonah Brüchert"), QString(), QStringLiteral("jbb@kaidan.im"), QStringLiteral("jbbgameich.github.io"));
    aboutData.addAuthor(i18n("Simon Schmeißer"));
    aboutData.setOrganizationDomain("kde.org");
    aboutData.setDesktopFileName(QStringLiteral("org.kde.qrca"));
    // set the application metadata
    KAboutData::setApplicationData(aboutData);
    // in GUI apps set the window icon manually, not covered by KAboutData
    // needed for environments where the icon name is not extracted from
    // the information in the application's desktop file
    QApplication::setWindowIcon(QIcon::fromTheme(QStringLiteral("qrca")));
    aboutData.setProgramLogo(app.windowIcon());

    qmlRegisterType<QrCodeScannerFilter>("org.kde.qrca", 1, 0, "QrCodeScannerFilter");

    Qrca qrca;
    qrca.setAboutData(aboutData);
    qrca.setEncodeText(parser.value(QStringLiteral("encode")));
    qmlRegisterSingletonInstance<Qrca>("org.kde.qrca", 1, 0, "Qrca", &qrca);

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextObject(new KLocalizedContext(&engine));

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
