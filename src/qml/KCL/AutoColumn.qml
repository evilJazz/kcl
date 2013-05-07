import QtQuick 1.0
import "private"

Column {
    id: column

    RowColumnAutoSizeLogic {
        id: logic
        orientation: Qt.Vertical
        target: column
    }

    property alias autoSizedChildren: logic.autoSizedChildren
    property alias blockUpdate: logic.blockUpdate
    property alias orientation: logic.orientation // READONLY!

    Component.onCompleted:
    {
        childrenChanged.connect(logic.updateNonAutoSizedChildren);
        logic.blockUpdate = false;
        logic.fullUpdate();
    }
}
