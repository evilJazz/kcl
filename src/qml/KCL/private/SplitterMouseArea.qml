import QtQuick 2.0
//import KCL 1.0 //@QtQuick1

MouseArea {
    id: mouseArea
    anchors.fill: parent

    cursorShape: splitterLogic.isHorizontal ? Qt.SplitVCursor : Qt.SplitHCursor //@QtQuick2

    property variant splitterLogic: logic

    Component.onCompleted:
    {
        if (mouseArea.hasOwnProperty("preventStealing"))
            mouseArea.preventStealing = true;
    }

    //CursorArea { //@QtQuick1
        //anchors.fill: parent //@QtQuick1
        //cursor: splitterLogic.isHorizontal ? CursorArea.SplitVCursor : CursorArea.SplitHCursor //@QtQuick1
    //} //@QtQuick1

    onPressed:
    {
        splitterLogic.start(mouse.x, mouse.y);
    }

    onPositionChanged:
    {
        if (mouseArea.pressed)
            splitterLogic.updateSiblings(mouse.x, mouse.y);
    }

    onReleased:
    {
        splitterLogic.finish(mouse.x, mouse.y);
    }
}
