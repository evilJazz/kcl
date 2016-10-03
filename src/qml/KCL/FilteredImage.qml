import QtQuick 2.0

Image {
    property real imageScaling: 1.0
    property url imageSource
    property bool inverted: false
    source: imageSource != "" ? "image://filtered/" + (inverted ? "invert," : "") + "scale=" + imageScaling + "@" + imageSource : ""
}
