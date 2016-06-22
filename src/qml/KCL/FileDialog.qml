import QtQuick 2.0

import "DeferredExecution.js" as DeferredExecution

QtObject {
    id: fileDialog

    signal accepted();
    signal rejected();

    property string caption: ""
    property string folder: ""
    property variant nameFilters: [{text: qsTr("All Files"), filter: ["*.*"]}, {text: qsTr("Pictures"), filter: ["*.jpg", "*.jpeg", "*.png"]}]

    property bool resolveSymLinks: true
    //property bool dirOnly: false

    property string result

    function getNativeFilter()
    {
        var preparedFilter = "";

        for (var i = 0; i < fileDialog.nameFilters.length; ++i)
        {
            var filter = fileDialog.nameFilters[i];

            var globbingFilter = "";

            for (var j = 0; j < filter.filter.length; ++j)
            {
                globbingFilter += filter.filter[j];
                if (j < filter.filter.length - 1)
                    globbingFilter += " ";
            }

            preparedFilter += filter.text + " (" + globbingFilter + ")";

            if (i < nameFilters.length - 1)
                preparedFilter += ";;"
        }
        return preparedFilter;
    }

    function open()
    {
        // Simulate non-modal operation with the native dialogs which are modal.
        DeferredExecution.invoke(function ()
        {
            var dialogResult = NativeDialogs.getOpenFileName(fileDialog.caption, fileDialog.folder, fileDialog.getNativeFilter(), fileDialog.resolveSymLinks);

            if (dialogResult.fileName.length > 0)
            {
                fileDialog.result = dialogResult.fileName;
                fileDialog.accepted();
            }
            else
            {
                fileDialog.result = "";
                fileDialog.rejected();
            }
        });
    }

    function save()
    {
        // Simulate non-modal operation with the native dialogs which are modal.
        DeferredExecution.invoke(function ()
        {
            var dialogResult = NativeDialogs.getSaveFileName(fileDialog.caption, fileDialog.folder, fileDialog.getNativeFilter(), fileDialog.resolveSymLinks);

            if (dialogResult.fileName.length > 0)
            {
                fileDialog.result = dialogResult.fileName;
                fileDialog.accepted();
            }
            else
            {
                fileDialog.result = "";
                fileDialog.rejected();
            }
        });
    }

    function existingDirectory()
    {
        // Simulate non-modal operation with the native dialogs which are modal.
        DeferredExecution.invoke(function ()
        {
            var dialogResult = NativeDialogs.getExistingDirectory(fileDialog.caption, fileDialog.folder, true, fileDialog.resolveSymLinks);

            if (dialogResult.length > 0)
            {
                fileDialog.result = dialogResult;
                fileDialog.accepted();
            }
            else
            {
                fileDialog.result = "";
                fileDialog.rejected();
            }
        });
    }
}
