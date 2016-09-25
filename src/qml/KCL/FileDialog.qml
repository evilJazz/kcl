/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

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
