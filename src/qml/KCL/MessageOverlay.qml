import QtQuick 1.0

import "qrc:/KCL/DeferredExecution.js" as DeferredExecution

Fader {
    id: overlay
    anchors.fill: parent
    color: "#e0000000"
    z: 10000
    opacity: 0

    property url source
    property Component sourceComponent

    property variant params
    property bool embedded: false

    property bool autoOpen: true
    property bool destroyOnDone: true

    property bool faderEnabled: true
    onFaderEnabledChanged: overlay.fadeAnimation.duration = faderEnabled ? 250 : 0

    property bool opened: false
    property bool done: false

    property alias frame: frame
    property alias item: loader.item

    onOpenedChanged: updateState()
    onSourceChanged: updateState()

    onDoneChanged:
    {
        if (done)
            overlay.close();
    }

    onFadedIn: opened = true;

    onFadedOut:
    {
        opened = false;
        if (done)
            unload();
    }

    Component.onCompleted:
    {
        if (autoOpen)
            open();
    }

    Timer {
        id: fadeTimer
        interval: 100
        repeat: false

        onTriggered:
        {
            if (loader.item !== null)
                overlay.fadeIn();
        }
    }

    function open()
    {
        if (loader.item == null)
        {
            overlay.done = false;

            if (overlay.source != "")
                loader.source = overlay.source;
            else
                loader.sourceComponent = overlay.sourceComponent;
        }
        else
        {
            if (faderEnabled)
                fadeTimer.start();
            else
                overlay.fadeIn();
        }
    }

    function close()
    {
        if (loader.item && overlay.opacity > 0)
            overlay.fadeOut();
        else
            unload();
    }

    function unload()
    {
        loader.sourceComponent = null;
        loader.source = "";

        if (destroyOnDone)
            // Use Queued invocation because destroy() may be called when still in Component.onCompleted, which causes
            // Error: Invalid attempt to destroy() an indestructible object in QtQuick 2+
            DeferredExecution.invoke(function () { overlay.destroy(); });
    }

    function updateState()
    {
        if (opened && opacity == 0)
            open();
        else if (!opened && opacity == 1)
            close();
    }

    MouseEater {
        anchors.fill: parent
    }

    Item {
        id: frame
        anchors.fill: parent
        anchors.margins: 30

        Loader {
            id: loader
            anchors.fill: parent
            onItemChanged:
            {
                if (item !== null)
                    overlay.open();
            }
        }
    }
}
