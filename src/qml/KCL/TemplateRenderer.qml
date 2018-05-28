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

    property string name
    property string childPrefix: parentRenderer ? parentRenderer.childPrefix : ""

    property string identifier: (parentRenderer ? parentRenderer.childPrefix + (parentRenderer.childPrefix.length > 0 ? "_" : "") : "") + name

    property url templateSource: ""
    property string templateText: ""
    property string content: ""
    property bool contentDirty: true

    property string templateRegEx: "\\${(.*?)}"

    property int renderDelay: 10 // Change to -1 to disable automatic rendering

    property QtObject topLevelRenderer: null

    signal subRendererTemplateChanged(var renderer)

    property variant _TemplateRenderer_ignoredPropertyNames: ["templateSource", "templateText", "templateRegEx", "contentDirty", "content", "renderDelay", "internalChildren", "children", "subRenderers", "topLevelRenderer", "parentRenderer", "debug", "markersUsedInTemplate"]
    ignoredPropertyNames: _TemplateRenderer_ignoredPropertyNames

    property variant markersUsedInTemplate: ({})
    function isMarkerUsedInTemplate(name)
    {
        return markersUsedInTemplate.hasOwnProperty(name);
    }

    property bool debug: false

    property Timer renderTimer: Timer {
        id: renderTimer
        interval: renderer.renderDelay
        repeat: false
        onTriggered: renderer.updateContent()
    }

    property QtObject registeredParentRenderer: null
    property QtObject parentRenderer: isTemplateRenderer(parent) ? parent : null
    onParentRendererChanged:
    {
        if (debug) Debug.print("New parent renderer: " + parentRenderer + " for " + renderer);

        if (registeredParentRenderer)
        {
            registeredParentRenderer.removeRenderer(renderer);
            registeredParentRenderer = null;
        }

        if (parentRenderer)
        {
            parentRenderer.addRenderer(renderer);
            registeredParentRenderer = parentRenderer;
        }

        if (debug) Debug.print(renderer +  " -> Parent renderer changed to " + parentRenderer + " This item's name: " + name);
        var newTopLevel = renderer;
        var object = parentRenderer;

        while (object && isTemplateRenderer(object))
        {
            newTopLevel = object;
            object = object.parentRenderer;
        }

        topLevelRenderer = newTopLevel;
        if (debug) Debug.print(renderer +  " -> parentRenderer: " + parentRenderer + " Top level template renderer: " + topLevelRenderer + " This item's name: " + name);
    }

    property Connections parentRendererConnections:
        Connections {
            target: parentRenderer
            onTopLevelRendererChanged: renderer.topLevelRenderer = parentRenderer.topLevelRenderer
        }

    onTemplateTextChanged: _TemplateRenderer_handleTemplateChanged()
    onTemplateSourceChanged:
    {
        templateText = ""; // Force reload
        _TemplateRenderer_handleTemplateChanged();
    }

    onTemplateRegExChanged: _TemplateRenderer_handleTemplateChanged()
    onSubRendererTemplateChanged: _notifyParentRenderer()

    function _TemplateRenderer_handleTemplateChanged()
    {
        if (templateText.length == 0 && templateSource != "")
            templateText = FsUtils.getContents(FsUtils.localPathFromUrl(templateSource));

        var usedMarkers = {};

        var re = new RegExp(renderer.templateRegEx, "gi");
        var match;
        while (match = re.exec(templateText))
        {
            var p1 = match[1];

            if (!usedMarkers.hasOwnProperty(p1))
                usedMarkers[p1] = 1;
            else
                ++usedMarkers[p1];
        }

        markersUsedInTemplate = usedMarkers;

        _notifyParentRenderer();
    }

    function _notifyParentRenderer()
    {
        _markContentDirty();

        if (isTemplateRenderer(parentRenderer))
            parentRenderer.subRendererTemplateChanged(renderer)
    }

    onPropertyChanged: _markContentDirty()

    function _markContentDirty()
    {
        if (!renderer.contentDirty)
            renderer.contentDirty = true;

        if (renderDelay == 0)
            updateContent();
        else if (renderDelay > 0)
            renderTimer.start();

        if (parentRenderer)
            parentRenderer._markContentDirty();
    }

    function _TemplateRenderer_replaceMarkerForProperty(propertyName)
    {
        return renderer[propertyName];
    }

    function replaceMarkerForProperty(propertyName)
    {
        return _TemplateRenderer_replaceMarkerForProperty(propertyName);
    }

    function _TemplateRenderer_replaceMarkerForContent()
    {
        return content;
    }

    function replaceMarkerForContent()
    {
        return _TemplateRenderer_replaceMarkerForContent();
    }

    function isTemplateRenderer(item)
    {
        return item && item.hasOwnProperty("contentDirty") &&
                       item.hasOwnProperty("parentRenderer")
    }

    function findTemplateRendererByName(name)
    {
        for (var i = 0; i < subRenderers.length; ++i)
        {
            var child = subRenderers[i];
            if (isTemplateRenderer(child) && child.name === name)
                return child;
        }

        return null;
    }

    function updateContent()
    {
        if (templateText.length == 0 && templateSource != "")
            templateText = FsUtils.getContents(FsUtils.localPathFromUrl(templateSource));

        var replacerCallback = function(match, p1, offset, string)
        {
            if (renderer.hasOwnProperty(p1))
                return replaceMarkerForProperty(p1);
            else
            {
                var subRenderer = findTemplateRendererByName(p1);
                if (subRenderer)
                {
                    if (subRenderer.contentDirty)
                        subRenderer.updateContent();

                    return subRenderer.replaceMarkerForContent();
                }
                else
                    return match;
            }
        }

        var re = new RegExp(renderer.templateRegEx, "gi");
        content = templateText.replace(re, replacerCallback);

        renderer.contentDirty = false;
    }

    // The following is a hack to allow a list property to be default...
    default property alias children: renderer.internalChildren
    property list<QtObject> internalChildren

    property variant subRenderers: []

    function addRenderer(childRenderer)
    {
        if (debug) Debug.print("Adding " + childRenderer + " to " + renderer);

        if (!childRenderer)
            return;

        var newChildren = subRenderers;
        var found = false;

        for (var i = 0; i < newChildren.length; ++i)
            if (newChildren[i] === childRenderer)
            {
                found = true;
                break;
            }

        if (!found)
        {
            newChildren.unshift(childRenderer);

            childRenderer.Component.destruction.connect(function()
            {
                if (renderer)
                    renderer.removeRenderer(childRenderer);
            });

            subRenderers = newChildren;
        }
    }

    function removeRenderer(childRenderer)
    {
        if (debug) Debug.print("Removing " + childRenderer + " from " + renderer);

        if (!childRenderer)
            return;

        var newChildren = subRenderers;

        for (var i = newChildren.length - 1; i >= 0; --i)
            if (newChildren[i] === childRenderer)
                newChildren.splice(i, 1);

        subRenderers = newChildren;
    }

    function dumpRendererStructure()
    {
        Debug.print(renderer + " name: " + renderer.name + " childPrefix: " + renderer.childPrefix + " identifier: " + renderer.identifier + " contentDirty: " + renderer.contentDirty);
        _dumpRendererStructure("    ");
    }

    function _dumpRendererStructure(indent)
    {
        if (typeof(indent) == "undefined")
            indent = "";

        var itemNr = 0;
        for (var i = 0; i < subRenderers.length; ++i)
        {
            var child = subRenderers[i];
            if (isTemplateRenderer(child))
            {
                Debug.print(indent + itemNr + ": " + child + " name: " + child.name + " childPrefix: " + child.childPrefix + " identifier: " + child.identifier + " contentDirty: " + child.contentDirty);
                child._dumpRendererStructure(indent + "    ");
                ++itemNr;
            }
        }
    }
}
