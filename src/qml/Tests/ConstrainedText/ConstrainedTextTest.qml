import QtQuick 1.0
import "../../KCL"

Rectangle {
    width: 300
    height: 400

    /*
    ConstrainedText {
        text: "Hello World!\nLorem Ipsum Dolor Sit Amet!!\nMenin aeide, thea, Peleïad(e)o Akhileos:\noulomenen, he muri' Akhaiois alge' etheke,"

        font.pixelSize: 30
        //anchors.fill: parent
        width: parent.width
        height: parent.height

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter
    }
    */

    ConstrainedText {
        id: backgroundText

        anchors.fill: parent
        anchors.margins: 10

        verticalAlignment: Text.AlignVCenter
        horizontalAlignment: Text.AlignHCenter

        font.pixelSize: 60

        text: "No song is currently playing.\nGet your groove on!"
    }

    MouseArea {
        anchors.fill: parent
        onClicked:
        {
            backgroundText.horizontalAlignment = Text.AlignLeft;
            backgroundText.horizontalAlignment = Text.AlignHCenter;
        }
    }
}
