import QtQuick 1.0

MouseArea {
    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
    onPressed:
    {
        mouse.accepted = true;
        forceActiveFocus();
    }

    onReleased: mouse.accepted = true;
    onClicked: mouse.accepted = true;
    onDoubleClicked: mouse.accepted = true;
}
