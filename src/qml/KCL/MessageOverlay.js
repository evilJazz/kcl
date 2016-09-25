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

function open(parent, delegate, params, autoOpen, looseItem, immediate)
{
    try
    {
        var component = Qt.createComponent(Qt.resolvedUrl("qrc:/KCL/MessageOverlay.qml"));
        if (component.status === Component.Error)
        {
            Debug.print(component.errorString());
            return null;
        }

        var p;
        if (Qt.isQtObject(delegate))
            p = {"sourceComponent": delegate, "params": params};
        else
            p = {"source": delegate};

        if (typeof(params) != "undefined")
            p.params = params;

        if (typeof(autoOpen) != "undefined")
            p.autoOpen = autoOpen;

        if (typeof(looseItem) != "undefined")
            p.looseItem = looseItem;

        if (typeof(immediate) != "undefined")
            p.fadeEnabled = !immediate;

        var overlay = component.createObject(parent, p);
        return overlay;
    }
    catch (e)
    {
        Debug.print(JSON.stringify(e));
        return null;
    }
}
