import QtQuick 1.0

Rectangle {
    Behavior on opacity { id: opacityBehaviour; NumberAnimation { id: animation; duration: 250 } }

    property alias fadeAnimation: animation
    property alias fadeEnabled: opacityBehaviour.enabled

    signal fadingIn
    signal fadedIn
    signal fadingOut
    signal fadedOut

    onOpacityChanged:
    {
        if (opacity == 1)
            fadedIn();
        else if (opacity == 0)
        {
            visible = false;
            fadedOut();
        }
    }

    function fadeIn()
    {
        visible = true;
        fadingIn();
        opacity = 1;
    }

    function fadeOut()
    {
        fadingOut();
        opacity = 0;
    }
}
