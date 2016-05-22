import QtQuick 2.0
import "private"

Rectangle {
    id: splitter

    property int splitterSize: 2

    width: isHorizontal ? parent.width : splitterSize
    height: isHorizontal ? splitterSize : parent.height

    property real position: isHorizontal ? splitter.y : splitter.x

    property alias autoAttachToSiblings: logic.autoAttachToSiblings
    property alias itemBefore: logic.itemBefore
    property alias itemAfter: logic.itemAfter

    property alias isHorizontal: logic.isHorizontal
    property alias orientation: logic.orientation

    Component.onCompleted: logic.initFromTargetParent()
    onParentChanged: logic.initFromTargetParent()

    color: "#AAAAAA"

    function setPosition(pos)
    {
        logic.setPosition(pos);
    }

    SplitterLogic {
        id: logic
        target: splitter
    }

    SplitterMouseArea {
        id: splitterMouseArea
        splitterLogic: logic
    }
}
