import QtQuick 2.0

QtObject {
    id: internal

    property bool debug: false

    property int startX
    property int startY

    property Item itemBefore
    property Item itemAfter

    property real startSizeBefore
    property real startSizeAfter

    property bool autoAttachToSiblings: true
    property bool parentCanBlockUpdate: false

    property Item target
    property Item connectedParent: null

    property bool isHorizontal: orientation == Qt.Horizontal
    property int orientation: Qt.Horizontal

    Component.onDestruction:
    {
        disconnectConnectedParent();
    }

    function disconnectConnectedParent()
    {
        if (connectedParent)
        {
            connectedParent.childrenChanged.disconnect(internal.initFromSiblings);
            connectedParent = null;
        }
    }

    function initFromTargetParent()
    {
        disconnectConnectedParent();

        if (target.parent)
        {
            parentCanBlockUpdate = parent.hasOwnProperty("blockUpdate");

            if (debug) console.log("parent: " + parent + " parent.orientation: " + target.parent.orientation + " parentCanBlockUpdates: " + parentCanBlockUpdate);

            if (target.parent.hasOwnProperty("orientation"))
                orientation = (target.parent.orientation === Qt.Vertical ? Qt.Horizontal : Qt.Vertical);

            connectedParent = target.parent;
            connectedParent.childrenChanged.connect(internal.initFromSiblings);

            initFromSiblings();
        }
    }

    function initFromSiblings()
    {
        if (debug) console.log("----------------------------");

        if (target.parent && autoAttachToSiblings)
        {
            var lastSibling;
            var sibling = null;

            for (var i = 0; i < target.parent.children.length - 1; ++i)
            {
                lastSibling = sibling;
                sibling = target.parent.children[i];
                if (debug) console.log("lastSibling: " + lastSibling + " sibling: " + sibling);

                if (sibling === target)
                {
                    itemBefore = lastSibling;
                    itemAfter = target.parent.children[i + 1];

                    if (debug) console.log("itemBefore: " + itemBefore + " itemAfter: " + itemAfter);
                }
            }
        }
    }

    function setPosition(pos)
    {
        if (parentCanBlockUpdate)
            target.parent.blockUpdate = true;

        var diff = pos - position;

        if (isHorizontal)
        {
            if (itemBefore) itemBefore.height = Math.max(1, itemBefore.height + diff);
            if (itemAfter) itemAfter.height = Math.max(1, itemAfter.height - diff);
        }
        else
        {
            if (itemBefore) itemBefore.width = Math.max(1, itemBefore.width + diff);
            if (itemAfter) itemAfter.width = Math.max(1, itemAfter.width - diff);
        }

        if (parentCanBlockUpdate)
            target.parent.blockUpdate = false;
    }

    function start(mouseX, mouseY)
    {
        if (parentCanBlockUpdate)
            target.parent.blockUpdate = true;

        var sceneMousePos = mapToItem(null, mouseX, mouseY);

        startX = sceneMousePos.x;
        startY = sceneMousePos.y;

        if (internal.isHorizontal)
        {
            if (itemBefore) startSizeAfter = itemBefore.height;
            if (itemAfter) startSizeBefore = itemAfter.height;
        }
        else
        {
            if (itemBefore) startSizeAfter = itemBefore.width;
            if (itemAfter) startSizeBefore = itemAfter.width;
        }
    }

    function updateSiblings(mouseX, mouseY)
    {
        var sceneMousePos = target.mapToItem(null, mouseX, mouseY);

        var diffX = sceneMousePos.x - startX;
        var diffY = sceneMousePos.y - startY;
        if (debug) console.log("diffX: " + diffX + " diffY: " + diffY + " sceneMousePos.x: " + sceneMousePos.x + " sceneMousePos.y: " + sceneMousePos.y);

        if (internal.isHorizontal)
        {
            if (itemBefore) itemBefore.height = Math.max(1, startSizeAfter + diffY);
            if (itemAfter) itemAfter.height = Math.max(1, startSizeBefore - diffY);
        }
        else
        {
            if (itemBefore) itemBefore.width = Math.max(1, startSizeAfter + diffX);
            if (itemAfter) itemAfter.width = Math.max(1, startSizeBefore - diffX);
        }
    }

    function finish(mouseX, mouseY)
    {
        updateSiblings(mouseX, mouseY);

        if (parentCanBlockUpdate)
            target.parent.blockUpdate = false;
    }
}
