/*
 *  Copyright (C) 2019 Simon Schmeisser <s.schmeisser@gmx.net>
 *
 *  Qrca is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  Qrca is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this project.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.6

import org.kde.kirigami 2.6 as Kirigami
import org.kde.qrca 1.0

Kirigami.AboutPage
{
    aboutData: Qrca.aboutData
}
