import QtQuick 1.0

Flickable {
    id: flicker
    contentHeight: column.height

    default property alias data: column.data
    property alias children: column.children
    property alias resources: column.resources

    property alias spacing: column.spacing

    property int decoratorWidth: 0
    property alias column: column
    property alias mouseEater: mouseEater

    interactive: height <= contentHeight

    MouseEater {
        id: mouseEater
        width: parent.width - decoratorWidth
        height: Math.max(column.implicitHeight, flicker.height)

        Column {
            id: column
            width: parent.width
        }
    }
}
