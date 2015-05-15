import QtQuick 1.0

QtObjectWithChildren {
    id: action
    property string text: ""
    property string tooltip: ""
    property string shortcut: ""
    property url iconSource: ""
    property bool enabled: true
    property bool checked: false
    property bool checkable: false
    property bool visible: true

    property int widthHint: 0
    property int heightHint: 0

    // ATTENTION: if you add properties or signals here, also update ActionForwarder !!!

    property bool isRunning: false

    signal triggered()
    function _Action_trigger()
    {
        if (isRunning) return;
        isRunning = true;

        try
        {
            triggered();
        }
        finally
        {
            isRunning = false;
        }
    }
    function trigger() { _Action_trigger(); }

    function _Action_toggle() { checked = !checked; }
    function toggle() { _Action_toggle(); }
}
