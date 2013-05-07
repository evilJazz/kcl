import QtQuick 1.0

Flickable {
    id: flicker
    contentHeight: column.height

    default property alias data: column.data

    property int decoratorWidth: 0

    Column {
        id: column
        width: parent.width - decoratorWidth
    }
}
