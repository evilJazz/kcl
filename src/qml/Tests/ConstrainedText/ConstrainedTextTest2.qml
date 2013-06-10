import QtQuick 1.0
import "../../KCL"

Column {
    width: 1000
    height: 400

    ConstrainedText {
        text: "Hello World!"

        font.pixelSize: 30
        anchors.left: parent.left
        anchors.right: parent.right

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }

    ConstrainedText {
        text: "Lorem ispum dolor sit amet."

        font.pixelSize: 30
        width: parent.width

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignRight
    }

    ConstrainedText {
        text: "Lorem ispum dolor sit amet."

        font.pixelSize: 60
        anchors.left: parent.left
        anchors.right: parent.right

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignLeft
    }

    /*
    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        color: "red"

        height: 100

        Rectangle {
            width: parent.width / scale
            height: parent.height / scale

            scale: 0.4
            transformOrigin: Item.TopLeft
            color: "blue"
        }
    }
    */
}
