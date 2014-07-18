import QtQuick 1.0
import KCL 1.0

import "private/GroupedItemGrid.js" as Private
import "qrc:/KCL/DeferredExecution.js" as DeferredExecution

Column {
    id: root

    // This repeater expects a model that provides
    // "group"    on the first/group level &
    // "itemData" on the second/item level

    // Use QObjectListModel for best results...

    property int itemsPerRow
    property int spacing: 5

    property int cellWidth: width / itemsPerRow - spacing
    property int cellHeight: cellWidth

    property int fullCellWidth: cellWidth + spacing
    property int fullCellHeight: cellHeight + spacing

    property alias model: groupRepeater.model

    // group object is attached via "groupModelData" property
    property Component headerDelegate

    // item object is attached via "itemModelData" property
    property Component itemDelegate

    // when the space between delegates is clicked
    signal clicked()

    property Flickable flickable

    function groupIndexFromItemId(itemId)
    {
        var sepPos = itemId.indexOf(">");
        return sepPos > -1 ? parseInt(itemId.slice(2, sepPos)) : -1;
    }

    function itemIndexFromItemId(itemId)
    {
        var sepPos = itemId.indexOf(">");
        return sepPos > -1 ? parseInt(itemId.slice(sepPos + 1)) : -1;
    }

    function itemId(groupIndex, itemIndex)
    {
        return "id" + groupIndex + ">" + itemIndex;
    }

    function firstItemId()
    {
        if (model.count > 0)
        {
            if (model.get(0).count > 0)
                return itemId(0, 0);
            else
                return nextItemId(itemId(0, 0));
        }
        else
            return false;
    }

    function lastItemId()
    {
        if (model.count > 0)
        {
            var lastItemId = itemId(model.count - 1, model.get(model.count - 1).count - 1);

            if (model.get(model.count - 1).count > 0)
                return lastItemId;
            else
                return prevItemId(lastItemId);
        }
        else
            return false;
    }

    function nextItemId(curItemId)
    {
        var groupIndex = groupIndexFromItemId(curItemId);
        var itemIndex = itemIndexFromItemId(curItemId);

        //console.log("groupIndex: " + groupIndex + " itemIndex: " + itemIndex);

        if (groupIndex > -1 && itemIndex > -1)
        {
            var group = model.get(groupIndex);
            if (!group) return false;

            if (itemIndex + 1 < group.count)
                return itemId(groupIndex, itemIndex + 1);
            else
            {
                ++groupIndex;

                while (groupIndex < model.count && model.get(groupIndex).count == 0)
                    ++groupIndex;

                if (groupIndex < model.count)
                    return itemId(groupIndex, 0);
                else
                    return false;
            }
        }
        else
            return false;
    }

    function prevItemId(curItemId)
    {
        var groupIndex = groupIndexFromItemId(curItemId);
        var itemIndex = itemIndexFromItemId(curItemId);

        //console.log("groupIndex: " + groupIndex + " itemIndex: " + itemIndex);

        if (groupIndex > -1 && itemIndex > -1)
        {
            var group = model.get(groupIndex);
            if (!group) return false;

            if (itemIndex - 1 >= 0)
                return itemId(groupIndex, itemIndex - 1);
            else
            {
                --groupIndex;

                while (groupIndex >= 0 && model.get(groupIndex).count == 0)
                    --groupIndex;

                if (groupIndex >= 0)
                    return itemId(groupIndex, model.get(groupIndex).count - 1);
                else
                    return false;
            }
        }
        else
            return false;
    }

    function itemIdCompare(leftItemId, rightItemId)
    {
        var leftGroupIndex = groupIndexFromItemId(leftItemId);
        var rightGroupIndex = groupIndexFromItemId(rightItemId);

        if (leftGroupIndex < rightGroupIndex)
            return 1;
        else if (leftGroupIndex > rightGroupIndex)
            return -1;
        else
        {
            var leftItemIndex = itemIndexFromItemId(leftItemId);
            var rightItemIndex = itemIndexFromItemId(rightItemId);

            if (leftItemIndex < rightItemIndex)
                return 1;
            else if (leftItemIndex > rightItemIndex)
                return -1;
            else
                return 0;
        }
    }

    function getRange(itemId1, itemId2)
    {
        var id;
        var results = [];
        var compareResult = itemIdCompare(itemId1, itemId2);

        if (compareResult === -1) // itemId1 is bigger
        {
            id = itemId1;
            do
            {
                results.push(id);
            }
            while (id !== itemId2 && (id = root.prevItemId(id)) !== false); // reorder and suffer!
        }
        else if (compareResult === 1) // itemId2 is bigger
        {
            id = itemId1;
            do
            {
                results.push(id);
            }
            while (id !== itemId2 && (id = root.nextItemId(id)) !== false); // reorder and suffer!
        }
        else // both equal
            return [itemId1];

        return results;
    }

    /*
    function test()
    {
        console.log("group count: " + root.model.count);

        var range1 = root.getRange(root.firstItemId(), root.lastItemId());
        console.log(JSON.stringify(range1));

        var range2 = root.getRange(root.lastItemId(), root.firstItemId());
        console.log(JSON.stringify(range2));

        var range3 = root.getRange(root.lastItemId(), root.lastItemId());
        console.log(JSON.stringify(range3));
    }
    */

    function getByItemId(itemId)
    {
        var groupIndex = groupIndexFromItemId(itemId);
        var itemIndex = itemIndexFromItemId(itemId);

        if (groupIndex > -1 && itemIndex > -1)
            return model.get(groupIndex).get(itemIndex);
        else
            return false;
    }

    QtObjectWithChildren {
        id: logic

        property bool debug: false

        Connections {
            target: root.flickable

            onContentYChanged: logic.deferredUpdateLoadState()
            onContentHeightChanged: logic.deferredUpdateLoadState()
        }

        property bool modelLoading: false
        property bool deferredUpdateStarted: false

        function deferredUpdateLoadState()
        {
            if (logic.deferredUpdateStarted || logic.modelLoading || root.height < 0 || root.flickable.contentHeight < 0) return;

            if (logic.debug) console.log("deferredUpdateLoadState() !!! height: " + height + " contextY: " + contentY + " contentHeight: " + contentHeight);

            deferredUpdateStarted = true;
            DeferredExecution.invoke(function ()
            {
                deferredUpdateStarted = false;
                logic.updateLoadState();
            },
            "deferredUpdateLoadState");
        }

        function itemVisible(item)
        {
            var posInView = root.flickable.mapFromItem(item, 0, 0);
            var visible = posInView.y + item.height >= 0 && posInView.y <= root.flickable.height;

            //if (logic.debug) console.log(item + " -> " + JSON.stringify(posInView) + " visible: " + visible);

            return visible;
        }

        function itemPosX(itemIndex)
        {
            return (itemIndex % root.itemsPerRow) * root.fullCellWidth;
        }

        function itemPosY(itemIndex)
        {
            return Math.floor(itemIndex / root.itemsPerRow) * root.fullCellHeight;
        }

        function itemIndexVisible(itemGrid, itemIndex)
        {
            var posInView = root.flickable.mapFromItem(itemGrid, 0, itemPosY(itemIndex));
            var visible = posInView.y + root.fullCellHeight >= 0 && posInView.y <= root.flickable.height;

            //if (logic.debug) console.log(itemIndex + " -> " + JSON.stringify(posInView) + " visible: " + visible);

            return visible;
        }

        function updateLoadState()
        {
            var newItems = [];
            var newIds = {};

            if (logic.debug) console.log("updateLoadState cleanup BEFORE: " + Private.items.length);

            for (var i = 0; i < Private.items.length; ++i)
            {
                var item = Private.items[i];

                if (itemVisible(item))
                {
                    newItems.push(item);
                    newIds[item.itemId] = true;
                }
                else
                    item.destroy();
            }

            if (logic.debug) console.log("updateLoadState cleanup AFTER: " + newItems.length);

            for (i = 0; i < root.children.length; ++i)
            {
                var groupItem = root.children[i];
                if (logic.debug) console.log("i: " + i + " groupItem: " + groupItem);

                if (groupItem.hasOwnProperty("groupIndex"))
                {
                    if (itemVisible(groupItem))
                    {
                        var groupModelData = groupItem.groupModelData;

                        for (var j = 0; j < groupModelData.count; ++j)
                        {
                            var newItemId = root.itemId(groupItem.groupIndex, j);
                            //if (logic.debug) console.log("newItemId: " + newItemId);

                            if (!newIds.hasOwnProperty(newItemId) && itemIndexVisible(groupItem.itemGrid, j))
                            {
                                var newItem = createItem(groupItem, j, groupModelData.get(j), newItemId);
                                newItems.push(newItem);
                                newIds[newItemId] = true;
                            }
                        }
                    }
                }
            }

            if (logic.debug) console.log("updateLoadState DONE: " + newItems.length);

            Private.items = newItems;
            Private.itemsIds = newIds;
        }

        Component {
            id: itemLoader

            Loader {
                x: logic.itemPosX(itemIndex)
                y: logic.itemPosY(itemIndex)

                width: root.cellWidth
                height: root.cellHeight
                sourceComponent: root.itemDelegate

                property variant itemId
                property int groupIndex
                property int itemIndex
                property QtObject itemModelData
            }
        }

        function createItem(groupItem, itemIndex, itemModelData, itemId)
        {
            var props = {
                itemId: itemId,
                groupIndex: groupItem.groupIndex,
                itemIndex: itemIndex,
                itemModelData: itemModelData
            }

            var newItem = itemLoader.createObject(groupItem.itemGrid, props);
            newItem.visible = true;
            return newItem;
        }
    }

    Repeater {
        id: groupRepeater
        anchors.left: parent.left
        anchors.right: parent.right

        Rectangle {
            id: groupContainer
            width: groupRepeater.width
            height: header.height + itemGrid.height + itemGrid.anchors.topMargin

            property QtObject groupModelData: group
            property int groupIndex: index

            property alias itemGrid: itemGrid

            MouseArea {
                id: clickCatcher

                anchors.fill: parent

                onClicked:
                {
                    root.clicked();
                }
            }

            Loader {
                id: header
                anchors.top: parent.top
                anchors.left: parent.left
                anchors.right: parent.right

                height: item !== null ? item.implicitHeight : 0

                sourceComponent: root.headerDelegate

                property QtObject groupModelData: group
            }

            Item {
                id: itemGrid

                anchors.topMargin: spacing
                anchors.top: header.bottom
                anchors.left: parent.left
                anchors.right: parent.right

                property int rows: Math.ceil(group.count / root.itemsPerRow)
                height: rows * root.fullCellHeight
            }
        }
    }
}

