import QtQuick 1.0
import "private"

Row {
    id: row

    RowColumnAutoSizeLogic {
        id: logic
        orientation: Qt.Horizontal
        target: row
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

    Component.onDestruction:
    {
        childrenChanged.disconnect(logic.updateNonAutoSizedChildren);
    }
}
