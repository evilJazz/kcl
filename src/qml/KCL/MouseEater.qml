import QtQuick 1.0

MouseArea {
    hoverEnabled: true
    acceptedButtons: Qt.LeftButton | Qt.RightButton | Qt.MiddleButton
    onPressed: forceActiveFocus()
}
