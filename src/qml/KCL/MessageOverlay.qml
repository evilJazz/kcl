import QtQuick 1.0

import "qrc:/KCL/DeferredExecution.js" as DeferredExecution

MouseEater {
    id: overlayRoot
    anchors.fill: parent
    z: 10000

    enabled: opened

    Component.onCompleted:
    {
        if (autoOpen)
            overlayRoot.open();
    }

    property alias fadeAnimation: overlay.fadeAnimation
    property alias fadeEnabled: overlay.fadeEnabled

    property alias color: overlay.color
    property alias source: overlay.source
    property alias sourceComponent: overlay.sourceComponent

    property alias params: overlay.params
    property alias embedded: overlay.embedded

    property alias autoOpen: overlay.autoOpen
    property alias destroyOnDone: overlay.destroyOnDone

    property alias opened: overlay.opened
    property alias done: overlay.done
    onDoneChanged:
    {
        if (done)
        {
            // Make sure that done signal is emitted before fadedOut signal no matter what fadeEnabled is set to.
            DeferredExecution.invoke(overlayRoot.close);
        }
    }

    property alias frame: frame
    property alias item: loader.item
    property alias looseItem: overlay.looseItem

    signal backgroundClicked()

    signal fadingIn()
    signal fadedIn()
    signal fadingOut()
    signal fadedOut()

    onClicked: overlayRoot.backgroundClicked()

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
            if (fadeEnabled)
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
        // Defer execution of unload, so signal handlers still have
        // a chance to work with loader.item and possible results...
        DeferredExecution.invoke(function ()
        {
            loader.sourceComponent = null;
            loader.source = "";

            if (destroyOnDone)
                // Use Queued invocation because destroy() may be called when still in Component.onCompleted, which causes
                // Error: Invalid attempt to destroy() an indestructible object in QtQuick 2+
                DeferredExecution.invoke(function () { overlayRoot.destroy(); });
        });
    }

    Fader {
        id: overlay
        anchors.fill: parent
        color: "#e0000000"
        opacity: 0

        property url source
        property Component sourceComponent

        property variant params
        property bool embedded: false

        property bool autoOpen: true
        property bool destroyOnDone: true

        property bool opened: false
        property bool done: false

        property alias frame: frame
        property alias item: loader.item
        property bool looseItem: false // This can only be set on instantiation because it influences the Loader's implicit size which can not be reset

        onOpenedChanged: updateState()
        onSourceChanged: updateState()

        onFadedIn: overlayRoot.fadedIn();

        onFadedOut:
        {
            opened = false;
            if (done)
                overlayRoot.unload();

            overlayRoot.fadedOut();
        }

        onFadingIn:
        {
            opened = true;
            overlayRoot.fadingIn();
        }
        onFadingOut: overlayRoot.fadingOut()

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

        function updateState()
        {
            if (opened && opacity == 0)
                overlayRoot.open();
            else if (!opened && opacity == 1)
                overlayRoot.close();
        }

        Item {
            id: frame
            anchors.fill: parent
            anchors.margins: !overlay.looseItem ? 30 : 0

            Loader {
                id: loader
                anchors.fill: !overlay.looseItem ? parent : undefined

                // Aliases required here for when using a sourceComponent
                // This seems to be a context bug in QtQuick
                property alias frame: frame
                property alias overlay: overlay
                property alias overlayRoot: overlayRoot

                onItemChanged:
                {
                    if (item !== null)
                        overlayRoot.open();
                }
            }
        }
    }
}
