#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>

#include "QrCodeScanner.h"
#include "QrSkanner.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KDE");
    QCoreApplication::setOrganizationDomain("kde.org");
    QCoreApplication::setApplicationName("QrSkanner");

    qRegisterMetaType<QrCodeScanner*>("QrCodeScanner*");
    qmlRegisterType<QrCodeScanner>("org.kde.qrskanner", 1, 0, "QrCodeScanner");

    QQmlApplicationEngine engine;

    QrSkanner *qrSkanner = new QrSkanner();
    engine.rootContext()->setContextProperty("qrSkanner", qrSkanner);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
