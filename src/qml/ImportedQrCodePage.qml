/*
 *  SPDX-FileCopyrightText: 2026 Kai Uwe Broulik <kde@broulik.de>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Controls as Controls
import QtQuick.Layouts

import org.kde.kquickcontrolsaddons as KQC
import org.kde.kirigami as Kirigami

Kirigami.Page {
    id: page

    property alias image: image.image
    property alias tag: resultSheet.tag

    title: i18nc("@title:window Opened image/QR code", "Image")

    KQC.QImageItem {
        id: image
        anchors.fill: parent
        fillMode: KQC.QImageItem.PreserveAspectFit
    }

    ResultSheet {
        id: resultSheet
        visible: true
    }
}
