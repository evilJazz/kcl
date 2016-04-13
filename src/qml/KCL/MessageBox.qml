import QtQuick 2.0

import "MessageBox.js" as MessageBox
import "DeferredExecution.js" as DeferredExecution

QtObject {
    property int type: MessageBox.Question

    property string title
    property string text

    property int buttons: MessageBox.Yes | MessageBox.No
    property int defaultButton: MessageBox.NoButton

    property int clickedButton

    function open()
    {
        // Simulate non-modal operation with the native dialogs which are modal.
        DeferredExecution.executeIn(1, function ()
        {
            if (type == MessageBox.Question)
                clickedButton = NativeDialogs.question(title, text, buttons, defaultButton);
            else if (type == MessageBox.Information)
                clickedButton = NativeDialogs.information(title, text, buttons, defaultButton);
            else if (type == MessageBox.Warning)
                clickedButton = NativeDialogs.warning(title, text, buttons, defaultButton);
            else if (type == MessageBox.Critical)
                clickedButton = NativeDialogs.critical(title, text, buttons, defaultButton);

            closed(clickedButton);
        })
    }

    signal closed(int clickedButton)
}
