import QtQuick 2.0

QtObject {
    objectName: command
    property string command: ""
    property string text: ""

    // Reimplement this execute method if you need
    // to evaluate parameters...
    function execute(param)
    {
        executed();
    }

    signal executed
}
