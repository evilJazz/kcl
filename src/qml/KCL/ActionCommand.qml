import QtQuick 1.0

ActionForwarder {
    objectName: command
    property string command: ""

    function execute(param)
    {
        trigger();
    }
}
