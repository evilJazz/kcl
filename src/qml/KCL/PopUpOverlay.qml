import QtQuick 1.0

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
        return overlay;
    }

    function popUp()
    {
        overlay = _createOverlay();
        overlay.open();
        _positionPopUp(overlay.item);
    }

    function popUpAtPos(x, y)
    {
        overlay = _createOverlay();
        overlay.open();
        overlay.item.x = x;
        overlay.item.y = y;
    }

    function close()
    {
        if (overlay)
            overlay.done = true;
    }

    function _positionPopUp(item)
    {
        var topPos = popUpParent.mapFromItem(itemToPopupAt, itemToPopupAt.width, 0);
        var bottomPos = popUpParent.mapFromItem(itemToPopupAt, itemToPopupAt.width, itemToPopupAt.height);
        var popUpParentHeight = popUpParent.mapFromItem(popUpParent, 0, popUpParent.height).y;

        if (bottomPos.y + item.height + 20 < popUpParentHeight)
        {
            item.x = bottomPos.x - item.width;
            item.y = bottomPos.y;
        }
        else
        {
            item.x = topPos.x - item.width;
            item.y = topPos.y - item.height;
        }
    }
}
