import QtQuick 1.0

//*
Item {
    id: container

    property alias color: text.color
    property alias font: text.font
    property alias horizontalAlignment: text.horizontalAlignment
    property alias verticalAlignment: text.verticalAlignment

    property alias text: text.text

    /*
    Rectangle {
        color: "red"
        scale: text.scale
        transformOrigin: Item.TopLeft
        width: text.width
        height: text.height
    }
    */

    Text {
        id: text

        transformOrigin: Item.TopLeft

        onPaintedSizeChanged:
        {
            updateScale();
            updateImplicitSize();
            updateSizeFromContainer();
        }
    }

    ValueChangeWatcher {
        target: container
        propertyName: "width"
        onValueChanged:
        {
            updateScale();
            updateSizeFromContainer();
        }
    }

    ValueChangeWatcher {
        target: container
        propertyName: "height"
        onValueChanged:
        {
            updateScale();
            updateSizeFromContainer();
        }
    }

    ValueChangeWatcher {
        target: text
        propertyName: "scale"
        onValueChanged:
        {
            updateImplicitSize();
            updateSizeFromContainer();
        }
    }

    function updateScale()
    {
        //console.log("text.paintedWidth: " + text.paintedWidth + " container.width: " + container.width);
        var newScale = container.width > 0 && text.paintedWidth > container.width ? container.width / text.paintedWidth : 1.0;
        if (text.scale !== newScale)
            text.scale = newScale;
    }

    function updateSizeFromContainer()
    {
        var newHeight = container.height / text.scale;
        if (text.height != newHeight)
            text.height = newHeight;

        var newWidth = container.width / text.scale;
        if (text.width != newWidth)
            text.width = newWidth;
    }

    function updateImplicitSize()
    {
        var newHeight = text.implicitHeight * text.scale;
        if (container.implicitHeight !== newHeight)
            container.implicitHeight = newHeight;

        var newWidth = text.implicitWidth * text.scale;
        if (container.implicitWidth !== newWidth)
            container.implicitWidth = newWidth;
    }
}
//*/


/*
Text {
    property real targetFontPixelSize: 30

    elide: Text.ElideNone

    onTextChanged: refit()
    //onHeightChanged: refit()
    onWidthChanged: refit()

    function refit()
    {
        if (elide != Text.ElideNone)
            elide = Text.ElideNone;

        //var scale = 1.0;
        if (paintedWidth > width)
            font.pixelSize = targetFontPixelSize * (width / paintedWidth)
        else
            font.pixelSize = targetFontPixelSize;
    }
}
*/

/*
Item {
    id: container

    property real targetFontPixelSize: 30

    property alias color: text.color
    property alias font: text.font
    property alias horizontalAlignment: text.horizontalAlignment
    property alias verticalAlignment: text.verticalAlignment

    property alias text: text.text

    Text {
        id: text
        width: parent["width"] / text["scale"]
        height: parent["height"] / text["scale"]

        transformOrigin: Item.TopLeft

        onPaintedSizeChanged: updateImplicitSize()
        onScaleChanged: updateImplicitSize()//implicitSizeUpdateTimer.restart()
    }

    // Using timer to avoid binding loop warnings...
    Timer {
        id: implicitSizeUpdateTimer
        repeat: false
        interval: 1
        onTriggered: updateImplicitSize()
    }

    function updateImplicitSize()
    {
        var newHeight = text["implicitHeight"] * text["scale"];
        if (newHeight !== container["implicitHeight"])
            container["implicitHeight"] = newHeight;

        var newWidth = text.implicitWidth * text.scale;
        if (newWidth !== container["implicitWidth"])
            container["implicitWidth"] = newWidth;

        //console.log("text.scale: " + text.scale + " implicitWidth: " + container["implicitWidth"] + " implicitHeight: " + container["implicitHeight"]);
    }

    states: [
        State {
            name: "normalsize"
            when: text.paintedWidth <= container.width

            PropertyChanges {
                target: text
                scale: 1.0
            }
        },
        State {
            name: "constrainedsize"
            when: text.paintedWidth > container.width

            PropertyChanges {
                target: text
                scale: container.width / text.paintedWidth
            }
        }
    ]
}
//*/

/*
Item {
    id: container

    property alias color: text.color
    property alias font: text.font
    property alias horizontalAlignment: text.horizontalAlignment
    property alias verticalAlignment: text.verticalAlignment

    property alias text: text.text

    Text {
        id: displayText
        width: parent.width / scale
        height: parent.height / scale

        transformOrigin: Item.TopLeft

        color: text.color
        font: text.font
        horizontalAlignment: text.horizontalAlignment
        verticalAlignment: text.verticalAlignment

        text: text.text

        onPaintedSizeChanged: updateImplicitSize()
        onScaleChanged: implicitSizeUpdateTimer.restart()
    }

    // Using timer to avoid binding loop warnings...
    Timer {
        id: implicitSizeUpdateTimer
        repeat: false
        interval: 1
        onTriggered: updateImplicitSize()
    }

    //Component.onCompleted: updateImplicitSize()

    function updateImplicitSize()
    {
        var newHeight = text.implicitHeight * displayText.scale;
        if (newHeight !== container.implicitHeight)
            container.implicitHeight = newHeight;

        var newWidth = text.implicitWidth * displayText.scale;
        if (newWidth !== container.implicitWidth)
            container.implicitWidth = newWidth;
    }

    Text {
        id: text
        visible: false
    }

    states: [
        State {
            name: "normalsize"
            when: text.paintedWidth <= container.width && text.paintedHeight <= container.height

            PropertyChanges {
                target: displayText
                scale: 1.0
            }
        },
        State {
            name: "constrainedsize"
            when: text.paintedWidth > container.width || text.paintedHeight > container.height

            PropertyChanges {
                target: displayText
                scale: container.width / text.paintedWidth
            }
        }
    ]
}
//*/
