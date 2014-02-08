import QtQuick 1.0

QtObject {
    id: logic

    property bool debug: false

    property Item target: null

    property real targetWidth: target ? target.width : 0
    onTargetWidthChanged: if (isHorizontal) fullUpdate()

    property real targetHeight: target ? target.height : 0
    onTargetHeightChanged: if (!isHorizontal) fullUpdate()

    property list<Item> autoSizedChildren

    property int orientation: Qt.Vertical
    property bool isHorizontal: orientation == Qt.Horizontal
    property real visibleNonAutoSizedChildrenSize
    property bool blockUpdate: true

    Component.onDestruction:
    {
        if (!target) return;
        
        for (var i = 0; i < target.children.length; ++i)
        {
            var item = target.children[i];
            disconnectAllSignals(item);
        }
    }

    onAutoSizedChildrenChanged: fullUpdate();

    function fullUpdate()
    {
        if (logic.blockUpdate) return;

        updateNonAutoSizedChildren();
        updateAutoSizedChildren();
    }

    function disconnectAllSignals(item)
    {
        if (isHorizontal)
            item.widthChanged.disconnect(logic.fullUpdate);
        else
            item.heightChanged.disconnect(logic.fullUpdate);

        item.visibleChanged.disconnect(logic.fullUpdate);
        item.opacityChanged.disconnect(logic.fullUpdate);
    }

    function updateNonAutoSizedChildren()
    {
        if (logic.blockUpdate) return;

        var newVisibleNonAutoSizedChildrenSize = 0;
        var item;
        var visibleCount = 0;
        var isAutoSizedChild;

        for (var i = 0; i < target.children.length; ++i)
        {
            item = target.children[i];

            isAutoSizedChild = false;

            for (var j = 0; j < autoSizedChildren.length; ++j)
            {
                if (autoSizedChildren[j] === item)
                {
                    isAutoSizedChild = true;
                    break;
                }
            }

            // disconnect from signals...
            disconnectAllSignals(item);

            // Analyze and connect to signals...

            var itemVisible = item.visible && item.opacity > 0 && (isHorizontal ? item.height : item.width) > 0;

            if (!isAutoSizedChild)
            {
                //newNonAutoSizedChildren.push(item);
                newVisibleNonAutoSizedChildrenSize += itemVisible ? (isHorizontal ? item.width : item.height) : 0;

                if (isHorizontal)
                    item.widthChanged.connect(logic.fullUpdate);
                else
                    item.heightChanged.connect(logic.fullUpdate);
            }

            if (itemVisible)
                ++visibleCount;

            item.visibleChanged.connect(logic.fullUpdate);
            item.opacityChanged.connect(logic.fullUpdate);
        }

        //nonAutoSizedChildren = newNonAutoSizedChildren;
        newVisibleNonAutoSizedChildrenSize += (visibleCount - 1) * spacing;
        visibleNonAutoSizedChildrenSize = newVisibleNonAutoSizedChildrenSize;

        if (debug)
        {
            console.log("autoSizedChildren.length: " + autoSizedChildren.length);
            //console.log("nonAutoSizedChildren.length: " + nonAutoSizedChildren.length);
            console.log("visibleNonAutoSizedChildrenSize: " + visibleNonAutoSizedChildrenSize);
        }
    }

    function updateAutoSizedChildren(doPercentageResize)
    {
        if (blockUpdate) return;

        if (debug) console.log("------------------------------------------------");

        var item, i;

        var autoSizeTotal = 0;
        var percentSizes = {};
        var invisiblePercentage = 0;
        var percentage;

        for (i = 0; i < autoSizedChildren.length; ++i)
        {
            item = autoSizedChildren[i];
            autoSizeTotal += (isHorizontal ? item.width : item.height)
        }

        if (debug) console.log("autoSizeTotal: " + autoSizeTotal);
        if (autoSizeTotal > 0)
        {
            for (i = 0; i < autoSizedChildren.length; ++i)
            {
                item = autoSizedChildren[i];

                percentage = (isHorizontal ? item.width : item.height) / autoSizeTotal;
                percentSizes[i] = percentage;
                if (!item.visible) invisiblePercentage += percentage;

                if (debug) console.log(item + ": " + percentSizes[i]);
            }
        }
        else
        {
            var unifiedPercentage = 1 / autoSizedChildren.length;

            for (i = 0; i < autoSizedChildren.length; ++i)
            {
                item = autoSizedChildren[i];

                percentSizes[i] = unifiedPercentage;
                if (!item.visible) invisiblePercentage += unifiedPercentage;

                if (debug) console.log(item + ": " + percentSizes[i]);
            }
        }

        if (invisiblePercentage < 1)
        {
            var remainingSize = (isHorizontal ? targetWidth : targetHeight) - visibleNonAutoSizedChildrenSize;
            var scaleRatio = 1 / (1 - invisiblePercentage);

            if (debug) console.log("remainingSize: " + remainingSize);

            for (i = 0; i < autoSizedChildren.length; ++i)
            {
                item = autoSizedChildren[i];
                if (isHorizontal)
                    item.width = remainingSize * percentSizes[i] * scaleRatio;
                else
                    item.height = remainingSize * percentSizes[i] * scaleRatio;
            }
        }
    }
}
