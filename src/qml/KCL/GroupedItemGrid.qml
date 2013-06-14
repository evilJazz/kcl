import QtQuick 1.0

Repeater {
    id: groupRepeater

    // This repeater expects a model that provides
    // "group"    on the first/group level &
    // "itemData" on the second/item level

    // Use QObjectListModel for best results...

    property int itemsPerRow

    // group object is attached via "groupModelData" property
    property Component headerDelegate

    // item object is attached via "itemModelData" property
    property Component itemDelegate

    Rectangle {
        width: groupRepeater.width
        height: header.height + itemGrid.height + itemGrid.anchors.topMargin

        Loader {
            id: header
            anchors.top: parent.top
            anchors.left: parent.left
            anchors.right: parent.right

            height: item !== null ? item.implicitHeight : 0

            sourceComponent: groupRepeater.headerDelegate

            property QtObject groupModelData: group
        }

        Grid {
            id: itemGrid

            anchors.topMargin: spacing
            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right

            spacing: 5

            columns: groupRepeater.itemsPerRow
            rows: Math.ceil(group.count / columns)

            property int cellWidth: itemGrid.width / columns - spacing
            property int cellHeight: cellWidth

            height: rows * (cellHeight + spacing)

            Repeater {
                model: group

                Loader {
                    width: itemGrid.cellWidth
                    height: itemGrid.cellHeight
                    sourceComponent: groupRepeater.itemDelegate

                    property QtObject itemModelData: itemData
                }
            }
        }
    }
}
