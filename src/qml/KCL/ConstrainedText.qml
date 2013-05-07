import QtQuick 1.0

Text {
    property real targetFontPixelSize: 10

    elide: Text.ElideNone

    onTextChanged: refit()
    //onHeightChanged: refit()
    onWidthChanged: refit()

    /*
    Timer {
        id: timer
        repeat: false
        interval: 1
        onTriggered: refit()
    }
    */

    function refit()
    {
        //console.log("refit: " + text);

        if (elide != Text.ElideNone)
            elide = Text.ElideNone;

        if (font.pixelSize != targetFontPixelSize)
            font.pixelSize = targetFontPixelSize;

        //if (quasarCore.isDebugBuild) console.log("width: " + width + " height: " + height + " text: " + text + " font.pixelSize: " + font.pixelSize);

        /*
        if (!width && !height || width < 0 || height < 0)
            return;

        if (width && height)
            while ((paintedWidth > width || paintedHeight > height) && font.pixelSize > 1)
                --font.pixelSize;
        else if (width)
        */
            while (paintedWidth > width && font.pixelSize > 1)
                --font.pixelSize;
        /*
        else if (height)
            while (paintedHeight > height && font.pixelSize > 1)
                --font.pixelSize;
        */

        //if (quasarCore.isDebugBuild) console.log("font.pixelSize NOW: " + font.pixelSize + " paintedWidth: " + paintedWidth + " paintedHeight: " + paintedHeight);
    }
}
