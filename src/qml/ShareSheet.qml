import QtQml
import QtQuick.Layouts

import org.kde.kirigami as Kirigami
import org.kde.purpose as Purpose

Kirigami.PromptDialog {
    id: shareSheet
    required property string text
    property var disabledPlugins: []
    implicitWidth: Kirigami.Units.gridUnit * 20
    title: i18nc("@title:window", "Share")
    Purpose.AlternativesView {
        id: alternativesView
        Layout.fillWidth: true
        pluginType: "ShareUrl"

        Component.onCompleted: {
            if ("disabledPlugins" in alternativesView) {
                alternativesView.disabledPlugins = shareSheet.disabledPlugins;
            }
        }
        inputData: {
            "urls": [shareSheet.text],
            "title": i18n("Scanned using Qrca")
        }
        onFinished: (output, error, message) => {
            shareSheet.close();
            if (error !== 0) {
                console.error("job finished with error", error, message);
            }
            alternativesView.reset();
            shareSheetLoader.source = "";
        }
    }
}
