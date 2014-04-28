import QtQuick 1.0

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
        var dialogResult = NativeDialogs.getOpenFileName(caption, folder, getNativeFilter(), resolveSymLinks);

        if (dialogResult.fileName.length > 0)
        {
            result = dialogResult.fileName;
            accepted();
        }
        else
        {
            result = "";
            rejected();
        }
    }

    function save()
    {
        var dialogResult = NativeDialogs.getSaveFileName(caption, folder, getNativeFilter(), resolveSymLinks);

        if (dialogResult.fileName.length > 0)
        {
            result = dialogResult.fileName;
            accepted();
        }
        else
        {
            result = "";
            rejected();
        }
    }

    function existingDirectory()
    {
        var dialogResult = NativeDialogs.getExistingDirectory(caption, folder, true, resolveSymLinks);

        if (dialogResult.length > 0)
        {
            result = dialogResult;
            accepted();
        }
        else
        {
            result = "";
            rejected();
        }
    }
}
