/*
 *  SPDX-FileCopyrightText: 2021 Jonah Brüchert <jbb@kaidan.im>
 *
 *  SPDX-License-Identifier: GPL-3.0-or-later
 */

import QtQuick 2.0
import QtQuick.Controls 2.3 as Controls
import org.kde.kirigami 2.7 as Kirigami
import QtQuick.Layouts 1.3
import org.kde.qrca 1.0

Kirigami.ScrollablePage {
    title: i18n("Saved QR Codes")
    ListView{
        model: GalleryModel{}
        delegate: Kirigami.BasicListItem{
            id: delegate
            required property string title
            required property string data
            text: title
            onClicked:{
                pageStack.layers.push("qrc:/DisplaySavedCode.qml", {"data": data, "title":title})
            }
        }

    }


}
