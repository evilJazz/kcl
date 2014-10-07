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

    function popUp()
    {
        var overlay = MessageOverlay.open(popUpParent, source != "" ? source : sourceComponent, {}, true, true);
        overlay.fadeEnabled = container.fadeEnabled;
        overlay.fadeAnimation.duration = container.fadeDuration;
        overlay.color = container.backgroundColor;

        overlay.fadingIn.connect(function () { container.aboutToPopUp(overlay.item) });
        overlay.fadedIn.connect(function () { container.poppedUp(overlay.item) });
        overlay.fadingOut.connect(function () { container.aboutToClose(overlay.item) });
        overlay.fadedOut.connect(function () { container.closed(overlay.item) });

        overlay.backgroundClicked.connect(function () { overlay.done = true; });

        overlay.open();
        positionPopUp(overlay.item);
    }

    function positionPopUp(item)
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
