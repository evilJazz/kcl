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

Action {
    id: forwarder

    property QtObject target: null

    property bool targetValid: target !== null
    onTargetValidChanged:
    {
        if (targetValid)
            bidiChecked.copyLeftToRight();
    }

    enabled: targetValid ? target.enabled : false
    visible: targetValid ? target.visible : false

    text: targetValid ? target.text : ""
    tooltip: targetValid ? target.tooltip : ""
    shortcut: targetValid ? target.shortcut : ""
    iconSource: targetValid ? target.iconSource : ""

    BidirectionalBinding {
        id: bidiChecked
        enabled: forwarder.targetValid
        leftTarget: target
        leftPropertyName: "checked"
        rightTarget: forwarder
        rightPropertyName: "checked"
    }

    checkable: targetValid ? target.checkable : false

    widthHint: targetValid ? target.widthHint : 0
    heightHint: targetValid ? target.heightHint : 0

    function toggle()
    {
        if (targetValid)
            target.toggle();
    }

    onTriggered: target.trigger()
}
