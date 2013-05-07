import QtQuick 1.0

Action {
    id: forwarder

    property QtObject target: null

    property bool targetValid: target !== null

    enabled: targetValid ? target.enabled : false
    visible: targetValid ? target.visible : false

    text: targetValid ? target.text : ""
    tooltip: targetValid ? target.tooltip : ""
    shortcut: targetValid ? target.shortcut : ""
    iconSource: targetValid ? target.iconSource : ""
    checked: targetValid ? target.checked : false
    checkable: targetValid ? target.checkable : false

    widthHint: targetValid ? target.widthHint : 0
    heightHint: targetValid ? target.heightHint : 0

    function toggle()
    {
        if (targetValid)
            target.toggle();
    }

    onTriggered: target.trigger()
}
