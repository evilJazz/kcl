import QtQuick 1.0
import "private"

Rectangle {
    id: splitter

    property int splitterSize: 2

    width: isHorizontal ? parent.width : splitterSize
    height: isHorizontal ? splitterSize : parent.height

    property alias autoAttachToSiblings: logic.autoAttachToSiblings
    property alias itemBefore: logic.before
    property alias itemAfter: logic.after

    property alias isHorizontal: logic.isHorizontal
    property alias orientation: logic.orientation

    Component.onCompleted: logic.initFromTargetParent()
    onParentChanged: logic.initFromTargetParent()

    color: "#AAAAAA"

    SplitterLogic {
        id: logic
        target: splitter
    }

    SplitterMouseArea {
        id: splitterMouseArea
        splitterLogic: logic
    }
}
