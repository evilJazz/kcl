import QtQuick 1.0

Flickable {
    id: flicker
    contentWidth: row.width

    default property alias data: row.data
    property alias children: row.children
    property alias resources: row.resources

    property alias spacing: row.spacing

    property int decoratorHeight: 0
    property alias row: row
    property alias mouseEater: mouseEater

    interactive: width <= contentWidth

    MouseEater {
        id: mouseEater
        height: parent.height - decoratorHeight
        width: Math.max(row.implicitWidth, flicker.width)

        Row {
            id: row
            height: parent.height
        }
    }
}
