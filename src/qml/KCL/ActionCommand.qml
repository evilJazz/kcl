import QtQuick 2.0

ActionForwarder {
    objectName: command
    property string command: ""

    function execute(param)
    {
        trigger();
    }
}
