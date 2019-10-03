import QtQuick 2.1
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Controls 2.0 as Controls

Kirigami.ApplicationWindow {
    id: root

    title: i18n("QR Code Scanner")

    function passiveNotification(text) {
        showPassiveNotification(text, "long")
    }

    Component.onCompleted: {
        qrca.passiveNotificationRequested.connect(passiveNotification)
        if (encodeText) {
            pageStack.layers.push(qrCodeEncoderPage)
        }
    }


    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("QR-Code Scanner")
        titleIcon: "view-barcode"

        actions: [
            Kirigami.Action {
                text: i18n("Scan")
                icon.name: "camera-photo-symbolic"
                onTriggered: pageStack.layers.pop()
            },
            Kirigami.Action {
                text: i18n("Create")
                icon.name: "document-new-symbolic"
                onTriggered: {
                    if (pageStack.layers.depth < 2)
                        pageStack.layers.push(qrCodeEncoderPage)
                }
            },
            Kirigami.Action {
                text: i18n("About")
                icon.name: "help-feedback"
                property string component: "qrc:/AboutPage.qml"
                onTriggered: if (pageStack.layers.depth < 2)
                           pageStack.layers.push(component)
            }

        ]
    }

    contextDrawer: Kirigami.ContextDrawer {}

    Component {id: qrCodeScannerPage; QrCodeScannerPage {}}
    Component {id: qrCodeEncoderPage; QrCodeEncoderPage {}}
    Component {id: aboutPage; AboutPage {}}

    pageStack.initialPage: qrCodeScannerPage
}
