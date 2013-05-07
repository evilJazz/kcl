import QtQuick 1.0

Rectangle {
    Behavior on opacity { NumberAnimation { duration: 250 } }

    signal fadingIn
    signal fadedIn
    signal fadingOut
    signal fadedOut

    onOpacityChanged:
    {
        if (opacity == 1)
            fadedIn();
        else if (opacity == 0)
            fadedOut();
    }

    function fadeIn()
    {
        fadingIn();
        opacity = 1;
    }

    function fadeOut()
    {
        fadingOut();
        opacity = 0;
    }
}
