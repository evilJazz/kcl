/****************************************************************************
**
** Copyright (C) 2017 Andre Beckedorf
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
import KCL 1.0

PropertyChangeObserver {
    id: renderer

    property url templateSource: ""
    property string template: ""
    property string content: ""
    property bool contentDirty: true

    property string templateRegEx: "\\${(.*?)}"

    property int renderDelay: 10 // Change to -1 to disable automatic rendering

    ignoredPropertyNames: ["contentDirty", "content", "renderDelay"]

    property Timer renderTimer: Timer {
        id: renderTimer
        interval: renderer.renderDelay
        repeat: false
        onTriggered: renderer.updateContent()
    }

    onParentChanged:
    {
        var newTopLevel = renderer;
        var item = parent;
        while (item && isTemplateRenderer(item))
        {
            newTopLevel = item;
            item = item.parent;
        }

        topLevelTemplateRenderer = newTopLevel;
    }

    property QtObject topLevelTemplateRenderer: null

    onPropertyChanged: _propertyChanged(propertyName)

    signal segmentChanged(string segmentName)

    function _segmentChanged(segmentName)
    {
        segmentChanged(segmentName);
        _propertyChanged(segmentName);
    }

    function _propertyChanged(propertyName)
    {
        if (!renderer.contentDirty)
            renderer.contentDirty = true;

        if (renderDelay == 0)
            updateContent();
        else if (renderDelay > 0)
            renderTimer.start();

        if (isTemplateRenderer(parent))
            parent._segmentChanged(renderer.name);
    }

    function _TemplateRenderer_replaceMarkerForProperty(propertyName)
    {
        return renderer[propertyName];
    }

    function replaceMarkerForProperty(propertyName)
    {
        return _TemplateRenderer_replaceMarkerForProperty(propertyName);
    }

    function isTemplateRenderer(item)
    {
        return item.hasOwnProperty("contentDirty") &&
               item.hasOwnProperty("name")
    }

    function findSegmentByName(name)
    {
        for (var i = 0; i < children.length; ++i)
        {
            var child = children[i];
            if (isTemplateRenderer(child) && child.name === name)
                return child;
        }

        return null;
    }

    function updateContent()
    {
        if (template.length == 0 && templateSource != "")
            template = FsUtils.getContents(FsUtils.localPathFromUrl(templateSource));

        var replacerCallback = function(match, p1, offset, string)
        {
            if (renderer.hasOwnProperty(p1))
                return replaceMarkerForProperty(p1);
            else
            {
                var segment = findSegmentByName(p1);
                if (segment)
                {
                    if (segment.contentDirty)
                        segment.updateContent();

                    return segment.content;
                }
                else
                    return match;
            }
        }

        var re = new RegExp(renderer.templateRegEx, "gi");
        content = template.replace(re, replacerCallback);

        renderer.contentDirty = false;
    }

    // The following is a hack to allow a list property to be default...
    default property alias children: renderer.internalChildren
    property list<QtObject> internalChildren
}
