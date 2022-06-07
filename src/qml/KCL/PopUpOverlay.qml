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

import "qrc:/KCL/MessageOverlay.js" as MessageOverlay;
import "private/PopUpOverlay.js" as Private

Item {
    id: container
    property Item itemToPopupAt: container
    onItemToPopupAtChanged: if (itemToPopupAt == null) close();

    property Component sourceComponent
    property url source

    property bool fadeEnabled: true
    property int fadeDuration: 50
    property color backgroundColor: "transparent"

    property Item popUpParent: app
    //property bool opened: overlay ? overlay.opened : false //@QtQuick1
    readonly property bool opened: overlay ? overlay.opened : false //@QtQuick2

    signal overlayCreated(variant item)
    signal aboutToPopUp(variant item)
    signal poppedUp(variant item)
    signal aboutToClose(variant item)
    signal closed(variant item)

    property variant overlay: null

    function _createOverlay(positionCallback)
    {
        var overlay = MessageOverlay.open(popUpParent, source != "" ? source : sourceComponent, {}, false, true);
        overlay.fadeEnabled = container.fadeEnabled;
        overlay.fadeAnimation.duration = container.fadeDuration;
        overlay.color = container.backgroundColor;

        overlay.fadingIn.connect(function () { container.aboutToPopUp(overlay.item) });
        overlay.fadedIn.connect(function () { container.poppedUp(overlay.item) });
        overlay.fadingOut.connect(function () { container.aboutToClose(overlay.item) });
        overlay.fadedOut.connect(function () { container.closed(overlay.item); container.overlay = null; });

        overlay.backgroundClicked.connect(function () { overlay.done = true; });

        overlay.itemChanged.connect(function () {
            if (overlay.item !== null)
            {
                overlayCreated(overlay.item);

                if (typeof(positionCallback) != "undefined")
                {
                    if (Private.currentPositionCallback != null)
                    {
                        _hookItemSize(parent, "disconnect", Private.currentPositionCallback);
                        _hookItemPos(itemToPopupAt, "disconnect", Private.currentPositionCallback);
                    }

                    Private.currentPositionCallback = positionCallback;

                    _hookItemSize(overlay.item, "connect", Private.currentPositionCallback);
                    _hookItemSize(parent, "connect", Private.currentPositionCallback);
                    _hookItemPos(itemToPopupAt, "connect", Private.currentPositionCallback);

                    Private.currentPositionCallback();
                }
            }
        });

        return overlay;
    }

    function _hookItemSize(item, method, callback)
    {
        if (item === null) return;
        item.heightChanged[method](callback);
        item.widthChanged[method](callback);
    }

    function _hookItemPos(item, method, callback)
    {
        if (item === null) return;
        item.xChanged[method](callback);
        item.yChanged[method](callback);
    }

    function popUp()
    {
        if (overlay == null)
            overlay = _createOverlay(function() { if (overlay !== null) _positionPopUpTopOrBottom(overlay.item); });

        overlay.open();
    }

    function popUpTopOrBottom()
    {
        popUp();
    }

    function popUpLeftOrRight()
    {
        if (overlay == null)
            overlay = _createOverlay(function() { if (overlay !== null) _positionPopUpLeftOrRight(overlay.item); });

        overlay.open();
    }

    function popUpAtPos(x, y)
    {
        if (overlay == null)
            overlay = _createOverlay(function () { if (overlay !== null) setBoundedPosition(overlay.item, x, y); });

        overlay.open();
        setBoundedPosition(overlay.item, x, y);
    }

    function close()
    {
        if (overlay)
            overlay.done = true;
    }

    function boundedPosition(item, x, y)
    {
        var popUpParentWidth = popUpParent.width;
        var popUpParentHeight = popUpParent.height;

        var result = {};
        result.x = Math.min(Math.max(0, x), popUpParentWidth - item.width);
        result.y = Math.min(Math.max(0, y), popUpParentHeight - item.height);

        return result;
    }

    function setBoundedPosition(item, x, y)
    {
        var pos = boundedPosition(item, x, y);
        item.x = pos.x;
        item.y = pos.y;
    }

    function popUpMove(x, y)
    {
        if (overlay && overlay.item)
            setBoundedPosition(overlay.item, x, y);
    }

    function popUpMoveTopOrBottom(item, itemRect)
    {
        if (!overlay || !overlay.item) return;

        var topPos = popUpParent.mapFromItem(item, itemRect.x + itemRect.width, itemRect.y);
        var bottomPos = popUpParent.mapFromItem(item, itemRect.x + itemRect.width, itemRect.y + itemRect.height);

        var popUpParentWidth = popUpParent.width;
        var popUpParentHeight = popUpParent.height;

        var x, y;

        if (bottomPos.y + overlay.item.height < popUpParentHeight)
        {
            x = bottomPos.x;
            y = bottomPos.y;
        }
        else
        {
            x = topPos.x;
            y = topPos.y - overlay.item.height;
        }

        setBoundedPosition(overlay.item, x, y);
    }

    function _positionPopUpTopOrBottom(item)
    {
        if (itemToPopupAt == null) return;
        popUpMoveTopOrBottom(itemToPopupAt, Qt.rect(0, 0, itemToPopupAt.width, itemToPopupAt.height));
    }

    function popUpMoveLeftOrRight(item, itemRect)
    {
        if (!overlay || !overlay.item) return;

        var topPos = popUpParent.mapFromItem(item, itemRect.x + itemRect.width, itemRect.y);
        var bottomPos = popUpParent.mapFromItem(item, itemRect.x + itemRect.width, itemRect.y + itemRect.height);

        var popUpParentWidth = popUpParent.width;
        var popUpParentHeight = popUpParent.height;

        var x, y;

        if (bottomPos.x + overlay.item.width < popUpParentWidth)
        {
            x = bottomPos.x;
            y = bottomPos.y;
        }
        else
        {
            x = topPos.x - overlay.item.width;
            y = topPos.y;
        }

        setBoundedPosition(overlay.item, x, y);
    }

    function _positionPopUpLeftOrRight(item)
    {
        if (itemToPopupAt == null) return;
        popUpMoveLeftOrRight(itemToPopupAt, Qt.rect(0, 0, itemToPopupAt.width, itemToPopupAt.height));
    }
}
