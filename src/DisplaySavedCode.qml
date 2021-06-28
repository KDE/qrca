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
    id: savedCodePage
    property string data
    ColumnLayout {
        Component.onCompleted: print(savedCodePage.data)
        Kirigami.AbstractCard {
            id: c
            Layout.fillWidth: true

            contentItem: GridLayout {
                id: layout

                Kirigami.Icon {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    Layout.preferredHeight: width

                    id: codeView
                    source: Qrca.encode(savedCodePage.data, layout.width)
                }
            }
        }
    }
}

