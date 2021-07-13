/***************************************************************************
 *                                                                         *
 *   SPDX-FileCopyrightText: 2019 Nicolas Fella <nicolas.fella@gmx.de>                   *
 *                                                                         *
 *   SPDX-License-Identifier: GPL-2.0-or-later
 *                                                                         *
 ***************************************************************************/

import QtQuick.Controls 2.1 as Controls
import QtQuick.Layouts 1.7
import QtQuick 2.7

import org.kde.kirigami 2.5 as Kirigami
import org.kde.purpose 1.0 as Purpose
import org.kde.qrca 1.0

Kirigami.OverlaySheet {
    id: inputSheet

    property var url
    property bool running: false
    property bool errorOccured: false

    leftPadding: 0
    rightPadding: 0

    header: Kirigami.Heading {
        text: i18n("Share Qr-Code")
        leftPadding: Kirigami.Units.largeSpacing
    }

    Controls.BusyIndicator {
        visible: inputSheet.running
        anchors.fill: parent
    }

    Kirigami.Heading {
        anchors.centerIn: parent
        width: parent.width - Kirigami.Units.gridUnit * 2
        text: i18n("Sharing Failed")
        horizontalAlignment: Text.AlignHCenter
        visible: errorOccured
    }

    Purpose.AlternativesView {
        id: view
        clip: true
        pluginType: "Export"
        implicitWidth: Kirigami.Units.gridUnit * 20
        implicitHeight: Kirigami.Units.gridUnit * 10

        delegate: Kirigami.BasicListItem {
            label: model.display
            icon: "arrow-right"
            onClicked: view.createJob(model.index)
            Keys.onReturnPressed: view.createJob(model.index)
            Keys.onEnterPressed: view.createJob(model.index)
        }

        onRunningChanged: inputSheet.running = running
        onFinished: {
            if (error === 0 && output.url && output.url.length !== 0) {
                // no error, display notification
                const resultUrl = output.url;
                NotificationManager.showNotification(resultUrl);
                Clipboard.content = resultUrl;
            }
            if (error === 0) {
                inputSheet.close();
                return;
            }
            errorOccured = true;
        }
    }

    onSheetOpenChanged: {
        errorOccured = false;
        if (sheetOpen) {
            view.inputData = {
                "urls": [inputSheet.url.toString()],
                "title": i18n("Share QR-Code"),
                "mimeType": "image"
            }
        } else {
            view.reset()
        }
    }
}

