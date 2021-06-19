/***************************************************************************
 *                                                                         *
 *   Copyright 2019 Nicolas Fella <nicolas.fella@gmx.de>                   *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA .        *
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

