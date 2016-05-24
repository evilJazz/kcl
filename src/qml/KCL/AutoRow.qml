import QtQuick 2.0
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

    property alias debug: logic.debug
    property alias _logic: logic

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
