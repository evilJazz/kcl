import QtQuick 1.0

import "qrc:/KCL/DeferredExecution.js" as DeferredExecution

MouseEater {
    id: overlayRoot
    anchors.fill: parent
    z: 10000

    property alias fadeAnimation: overlay.fadeAnimation
    property alias fadeEnabled: overlay.fadeEnabled
    property alias faderEnabled: overlay.fadeEnabled // TODO: Get rid of it

    property alias color: overlay.color
    property alias source: overlay.source
    property alias sourceComponent: overlay.sourceComponent

    property alias params: overlay.params
    property alias embedded: overlay.embedded

    property alias autoOpen: overlay.params
    property alias destroyOnDone: overlay.destroyOnDone

    property alias opened: overlay.opened
    property alias done: overlay.done

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
            DeferredExecution.invoke(function () { overlayRoot.destroy(); });
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

        onDoneChanged:
        {
            if (done)
                overlayRoot.close();
        }

        onFadedIn:
        {
            opened = true;
            overlayRoot.fadedIn();
        }

        onFadedOut:
        {
            opened = false;
            if (done)
                overlayRoot.unload();

            overlayRoot.fadedOut();
        }

        onFadingIn: overlayRoot.fadingIn()
        onFadingOut: overlayRoot.fadingOut()

        Component.onCompleted:
        {
            if (autoOpen)
                overlayRoot.open();
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

                onItemChanged:
                {
                    if (item !== null)
                        overlayRoot.open();
                }
            }
        }
    }
}
