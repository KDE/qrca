import QtQuick 2.0
import QtQuick.Controls 2.3 as Controls
import QtMultimedia 5.9
import org.kde.kirigami 2.2 as Kirigami
import QtQuick.Layouts 1.3

Kirigami.Page {
    ColumnLayout {
        anchors.fill: parent
        Kirigami.Icon {
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
