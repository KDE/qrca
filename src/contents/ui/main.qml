import QtQuick 2.1
import org.kde.kirigami 2.4 as Kirigami
import QtQuick.Controls 2.0 as Controls

Kirigami.ApplicationWindow {
    id: root

    title: i18n("QR Code Scanner")

    globalDrawer: Kirigami.GlobalDrawer {
        title: i18n("QR-Code Scanner")
        titleIcon: "view-barcode"

        actions: [
            Kirigami.Action {
                text: i18n("Scan")
                onTriggered: pageStack.replace(qrCodeScannerPage)
            },
            Kirigami.Action {
                text: i18n("Create")
                onTriggered: pageStack.replace(qrCodeEncoderPage)
            }
        ]
    }

    Component {id: qrCodeScannerPage; QrCodeScannerPage {}}
    Component {id: qrCodeEncoderPage; QrCodeEncoderPage {}}

    pageStack.initialPage: qrCodeScannerPage
}
