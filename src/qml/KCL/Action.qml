/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

import QtQuick 2.0

QtObjectWithChildren {
    id: action
    property string text: ""
    property string tooltip: ""
    property string shortcut: ""
    property url iconSource: ""
    property bool enabled: true
    property bool checked: false
    property bool checkable: false
    property bool visible: true

    property int widthHint: 0
    property int heightHint: 0

    // ATTENTION: if you add properties or signals here, also update ActionForwarder !!!

    property bool isRunning: false

    signal triggered()
    function _Action_trigger()
    {
        if (isRunning) return;
        isRunning = true;

        try
        {
            triggered();
        }
        finally
        {
            isRunning = false;
        }
    }
    function trigger() { _Action_trigger(); }

    function _Action_toggle() { checked = !checked; }
    function toggle() { _Action_toggle(); }
}
