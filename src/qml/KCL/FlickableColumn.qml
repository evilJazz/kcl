import QtQuick 1.0

Flickable {
    id: flicker
    contentHeight: column.height

    default property alias data: column.data

    property alias spacing: column.spacing

    property int decoratorWidth: 0
    property alias column: column

    Column {
        id: column
        width: parent.width - decoratorWidth
    }
}
