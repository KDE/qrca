import QtQuick 2.0
import QtQuick.Controls 2.3 as Controls
import QtMultimedia 5.9
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Layouts 1.3

Kirigami.ScrollablePage {
    title: i18n("Create QR-Code")

    ShareSheet {
        id: shareSheet
        title: i18n("Share QR-Code")
    }


    actions.contextualActions: [
        Kirigami.Action {
            property string path
            text: i18n("Save")
            onTriggered: {
                path = "file://" + qrSkanner.save(codeView.source)
                showPassiveNotification(i18n("Saved image to " + path), 1000, "Open Externally", function() {Qt.openUrlExternally(path)})
            }
        },
        Kirigami.Action {
            text: i18n("Share")
            onTriggered: {
                shareSheet.url = qrSkanner.save(codeView.source)
                shareSheet.open()
            }
        }
    ]

    ColumnLayout {
        spacing: 50

        Kirigami.Icon {
            id: codeView
            Layout.fillWidth: true
            width: Kirigami.Units.gridUnit * 20
            height: Kirigami.Units.gridUnit * 20
            source: inputText.text.length > 0 ? qrSkanner.encode(inputText.text, width) : ""
        }

        Controls.TextField {
            id: inputText
            text: "Enter text"
            Layout.fillWidth: true
        }
    }
}
