import QtQuick 1.0

MouseArea {
    id: mouseArea
    anchors.fill: parent

    property variant splitterLogic: logic

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
