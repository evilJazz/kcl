import QtQuick 1.0
import "private"

Rectangle {
    id: splitter

    width: isHorizontal ? parent.width : 10
    height: isHorizontal ? 10 : parent.height

    property alias autoAttachToSiblings: logic.autoAttachToSiblings
    property alias itemBefore: logic.before
    property alias itemAfter: logic.after

    property alias isHorizontal: logic.isHorizontal
    property alias orientation: logic.orientation

    Component.onCompleted: logic.initFromTargetParent()
    onParentChanged: logic.initFromTargetParent()

    gradient: Gradient {
        GradientStop {
            position: 0.00;
            color: "#626262";
        }
        GradientStop {
            position: 1.00;
            color: "#2d2d2d";
        }
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
