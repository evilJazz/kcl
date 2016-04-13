import QtQuick 2.0

QtObject {
    id: list
    // The following is a hack to allow a list property to be default...
    default property alias children: list.internalChildren
    property list<QtObject> internalChildren
}
