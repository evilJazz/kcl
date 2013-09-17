import QtQuick 1.1

import "TestJavaScript.js" as Test

// Import MagicRectangle component based on QtQuick version...
import "QtQuick1" //@QtQuick1
//import "QtQuick2" //@QtQuick2

Rectangle {
    width: 360
    height: 360

    Component.onCompleted: console.log(Test.SomeText);

    MagicRectangle {
        anchors.centerIn: parent
    }

    Text {
        text: qsTr("Hello World from QtQuick 1!") //@QtQuick1
        //text: qsTr("Hello World from QtQuick 2!") //@QtQuick2
        anchors.centerIn: parent
    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            Qt.quit();
        }
    }
}
