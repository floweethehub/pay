/*
 * This file is part of the Flowee project
 * Copyright (C) 2021 Tom Zander <tom@flowee.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
import QtQuick 2.11
import QtQuick.Controls 2.11

Label {
    elide: Text.ElideMiddle

    property string clipboardText: ""

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.RightButton
        onClicked:  menu.start(parent);

        Menu {
            id: menu
            function start(label) {
                if (label.clipboardText !== "")
                    this.text = label.clipboardText;
                else
                    this.text = label.text;
                popup();
            }
            property string text: ""
            MenuItem {
                text: qsTr("Copy")
                onTriggered:  Flowee.copyToClipboard(menu.text)
            }
        }
    }
}
