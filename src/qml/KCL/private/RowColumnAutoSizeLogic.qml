import QtQuick 1.0

QtObject {
    id: logic

    property bool debug: false

    property Item target: null

    property real targetWidth: target ? target.width : 0
    onTargetWidthChanged: fullUpdate()

    property real targetHeight: target ? target.height : 0
    onTargetHeightChanged: fullUpdate()

    property real targetSpacing: target ? target.spacing : 0
    onTargetSpacingChanged: fullUpdate()

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

        if (debug) console.log("updateNonAutoSizedChildren ------------------------------------------------");

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
            var itemSize = itemVisible ? (isHorizontal ? item.width : item.height) : 0;

            if (!isAutoSizedChild)
            {
                newVisibleNonAutoSizedChildrenSize += Math.floor(itemSize);

                if (isHorizontal)
                    item.widthChanged.connect(logic.fullUpdate);
                else
                    item.heightChanged.connect(logic.fullUpdate);
            }

            if (debug)
            {
                var suffix = itemSize - Math.floor(itemSize) > 0 ? "floored to " + Math.floor(itemSize) + "px " : "";
                console.log(item + ": " + itemSize + "px " + suffix + (isAutoSizedChild ? "autosized" : "") + " (visible: " + itemVisible + ")");
            }

            if (itemVisible)
                ++visibleCount;

            item.visibleChanged.connect(logic.fullUpdate);
            item.opacityChanged.connect(logic.fullUpdate);
        }

        var allSpacing = (visibleCount - 1) * spacing;
        newVisibleNonAutoSizedChildrenSize += allSpacing;
        visibleNonAutoSizedChildrenSize = newVisibleNonAutoSizedChildrenSize;

        if (debug)
        {
            console.log("allSpacing: " + allSpacing);
            console.log("autoSizedChildren.length: " + autoSizedChildren.length);
            console.log("visibleNonAutoSizedChildrenSize: " + visibleNonAutoSizedChildrenSize);
        }
    }

    function updateAutoSizedChildren(doPercentageResize)
    {
        if (blockUpdate) return;

        if (debug) console.log("updateAutoSizedChildren ------------------------------------------------");

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

                var itemSize = (isHorizontal ? item.width : item.height);

                percentage = itemSize / autoSizeTotal;
                percentSizes[i] = percentage;
                if (!item.visible) invisiblePercentage += percentage;

                if (debug) console.log(item + ": " + itemSize + "px -> " + percentSizes[i] * 100 + "%");
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

                if (debug) console.log(item + ": " + percentSizes[i] * 100 + "%");
            }
        }

        if (invisiblePercentage < 1)
        {
            var targetSize = (isHorizontal ? targetWidth : targetHeight);
            var remainingSize = targetSize - visibleNonAutoSizedChildrenSize;
            var scaleRatio = 1 / (1 - invisiblePercentage);

            if (debug) console.log("targetSize: " + targetSize + " visibleNonAutoSizedChildrenSize: " + visibleNonAutoSizedChildrenSize + " remainingSize: " + remainingSize);

            var usedSize = 0;
            var calculatedSize = 0;

            for (i = 0; i < autoSizedChildren.length; ++i)
            {
                item = autoSizedChildren[i];
                calculatedSize = remainingSize * percentSizes[i] * scaleRatio;

                if (isHorizontal)
                    item.width = calculatedSize;
                else
                    item.height = calculatedSize;

                usedSize += Math.floor(calculatedSize);
            }
        }
    }
}
