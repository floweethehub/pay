/*
 * This file is part of the Flowee project
 * Copyright (C) 2020 Tom Zander <tomz@freedommail.ch>
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
import QtQuick 2.14
import QtQuick.Controls 2.14
import Flowee.org.pay 1.0

// This is a silly hack to introduce a visual difference
// between enabled and disabled buttons.
Button {
    property var backupPalette: mainWindow.palette
    onEnabledChanged: {
        palette = backupPalette
        if (!enabled)
            palette.buttonText = Flowee.useDarkSkin
                    ? Qt.darker(palette.buttonText)
                    : Qt.lighter(palette.buttonText)
    }
}