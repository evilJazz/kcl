import QtQuick 2.0

import "qrc:/KCL/MessageOverlay.js" as MessageOverlay;

Item {
    id: container
    property Item itemToPopupAt: container

    property Component sourceComponent
    property url source

    property bool fadeEnabled: true
    property int fadeDuration: 50
    property color backgroundColor: "transparent"

    property Item popUpParent: app

    signal overlayCreated(variant item)
    signal aboutToPopUp(variant item)
    signal poppedUp(variant item)
    signal aboutToClose(variant item)
    signal closed(variant item)

    property variant overlay: null

    function _createOverlay()
    {
        var overlay = MessageOverlay.open(popUpParent, source != "" ? source : sourceComponent, {}, true, true);
        overlay.fadeEnabled = container.fadeEnabled;
        overlay.fadeAnimation.duration = container.fadeDuration;
        overlay.color = container.backgroundColor;

        overlay.fadingIn.connect(function () { container.aboutToPopUp(overlay.item) });
        overlay.fadedIn.connect(function () { container.poppedUp(overlay.item) });
        overlay.fadingOut.connect(function () { container.aboutToClose(overlay.item) });
        overlay.fadedOut.connect(function () { container.closed(overlay.item); container.overlay = null; });

        overlay.backgroundClicked.connect(function () { overlay.done = true; });

        overlayCreated(overlay.item);

        return overlay;
    }

    function popUp()
    {
        overlay = _createOverlay();
        overlay.open();
        _positionPopUpTopOrBottom(overlay.item);
    }

    function popUpTopOrBottom()
    {
        popUp();
    }

    function popUpLeftOrRight()
    {
        overlay = _createOverlay();
        overlay.open();
        _positionPopUpLeftOrRight(overlay.item);
    }

    function popUpAtPos(x, y)
    {
        overlay = _createOverlay();
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
        var popUpParentSize = popUpParent.mapFromItem(popUpParent, popUpParent.width, popUpParent.height);
        var popUpParentWidth = popUpParentSize.x;
        var popUpParentHeight = popUpParentSize.y;

        var result = {};
        result.x = x;
        result.y = y;

        if (x + item.width > popUpParentWidth)
            result.x = popUpParentWidth - item.width;

        if (y + item.height > popUpParentHeight)
            result.y = popUpParentHeight - item.height;

        return result;
    }

    function setBoundedPosition(item, x, y)
    {
        var pos = boundedPosition(item, x, y);
        item.x = pos.x;
        item.y = pos.y;
    }

    function _positionPopUpTopOrBottom(item)
    {
        var topPos = popUpParent.mapFromItem(itemToPopupAt, itemToPopupAt.width, 0);
        var bottomPos = popUpParent.mapFromItem(itemToPopupAt, itemToPopupAt.width, itemToPopupAt.height);

        var popUpParentSize = popUpParent.mapFromItem(popUpParent, popUpParent.width, popUpParent.height);
        var popUpParentWidth = popUpParentSize.x;
        var popUpParentHeight = popUpParentSize.y;

        var x, y;

        if (bottomPos.y + item.height < popUpParentHeight)
        {
            x = bottomPos.x - item.width;
            y = bottomPos.y;
        }
        else
        {
            x = topPos.x - item.width;
            y = topPos.y - item.height;
        }

        setBoundedPosition(item, x, y);
    }

    function _positionPopUpLeftOrRight(item)
    {
        var topLeftPos = popUpParent.mapFromItem(itemToPopupAt, 0, 0);
        var topRightPos = popUpParent.mapFromItem(itemToPopupAt, itemToPopupAt.width, 0);

        var popUpParentSize = popUpParent.mapFromItem(popUpParent, popUpParent.width, popUpParent.height);
        var popUpParentWidth = popUpParentSize.x;
        var popUpParentHeight = popUpParentSize.y;

        var x, y;

        if (topRightPos.x + item.width < popUpParentWidth)
        {
            x = topRightPos.x;
            y = topRightPos.y;
        }
        else
        {
            x = topLeftPos.x - item.width;
            y = topLeftPos.y;
        }

        setBoundedPosition(item, x, y);
    }
}
