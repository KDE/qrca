#include <QApplication>
#include <QQmlApplicationEngine>
#include <QtQml>
#include <QUrl>

#include "QrCodeScannerFilter.h"
#include "Qrca.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
    QGuiApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QApplication app(argc, argv);
    QCoreApplication::setOrganizationName("KDE");
    QCoreApplication::setOrganizationDomain("kde.org");
    QCoreApplication::setApplicationName("qrca");

    qmlRegisterType<QrCodeScannerFilter>("org.kde.qrca", 1, 0, "QrCodeScannerFilter");

    QQmlApplicationEngine engine;

    auto *qrca = new Qrca();
    engine.rootContext()->setContextProperty("qrca", qrca);

    engine.load(QUrl(QStringLiteral("qrc:///main.qml")));

    if (engine.rootObjects().isEmpty()) {
        return -1;
    }

    return app.exec();
}
