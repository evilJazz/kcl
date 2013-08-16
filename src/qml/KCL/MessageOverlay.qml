import QtQuick 1.0

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
            fadeTimer.start();
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
            overlay.destroy();
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
