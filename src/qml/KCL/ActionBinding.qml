import QtQuick 1.0

Action {
    id: action
    checkable: true
    property QtObject target
    property string propertyName

    checked: target[propertyName]
    onCheckedChanged: target[propertyName] = checked

    Binding {
        target: action
        property: "checked"
        value: action.target[action.propertyName]
    }
}
