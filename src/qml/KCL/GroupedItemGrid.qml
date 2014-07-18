import QtQuick 1.0

Repeater {
    id: groupRepeater

    property bool debugMessages: false

    // This repeater expects a model that provides
    // "group"    on the first/group level &
    // "itemData" on the second/item level

    // Use QObjectListModel for best results...

    property int itemsPerRow

    property int spacing: 5
    property int cellWidth: groupRepeater.width / itemsPerRow - spacing
    property int cellHeight: cellWidth

    property int fullCellWidth: cellWidth + spacing
    property int fullCellHeight: cellHeight + spacing

    // group object is attached via "groupModelData" property
    property Component headerDelegate

    // item object is attached via "itemModelData" property
    property Component itemDelegate

    // when the space between delegates is clicked
    signal clicked()

    Rectangle {
        id: groupContainer
        width: groupRepeater.width
        height: header.height + itemGrid.height + itemGrid.anchors.topMargin

        property QtObject groupModelData: group
        property int groupIndex: index

        MouseArea {
            id: clickCatcher

            anchors.fill: parent

            onClicked:
            {
                groupRepeater.clicked();
            }
        }

        Loader {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: item !== null ? item.implicitHeight : 0

            sourceComponent: groupRepeater.headerDelegate

            property QtObject groupModelData: group
        }

        Item {
            id: itemGrid

            anchors.topMargin: spacing
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            property int rows: Math.ceil(group.count / groupRepeater.itemsPerRow)
            height: rows * groupRepeater.fullCellHeight

            Component.onCompleted:
            {
                if (groupRepeater.debugMessages) console.log("Component.onCompleted: " + groupContainer.groupIndex + " group: " + groupContainer.groupModelData + " count: " + groupContainer.groupModelData.count);

                for (var i = 0; i < groupContainer.groupModelData.count; ++i)
                {
                    createItem(i);
                }
            }
        }

        function createItem(itemIndex)
        {
            var props = {
                itemIndex: itemIndex,
                itemModelData: groupContainer.groupModelData.get(itemIndex)
            }

            var newItem = itemLoader.createObject(itemGrid, props);
            newItem.visible = true;
        }

        Component {
            id: itemLoader

            Loader {
                x: (itemIndex % groupRepeater.itemsPerRow) * groupRepeater.fullCellWidth
                y: Math.floor(itemIndex / groupRepeater.itemsPerRow) * groupRepeater.fullCellHeight

                width: groupRepeater.cellWidth
                height: groupRepeater.cellHeight
                sourceComponent: groupRepeater.itemDelegate

                property int itemIndex
                property QtObject itemModelData
            }
        }
    }
}
