import QtQuick 1.1
import KCL 1.0

Rectangle {
    width: 360
    height: 360

    GridView {
        anchors.fill: parent
        cacheBuffer: height * 2
        model: 2000

        cellHeight: 160
        cellWidth: 160

        delegate: ImageRenderSurface {
            width: 160
            height: 160
            source: "image://test/red"

            opacity: status === ImageRenderSurface.Rendered ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 250 } }
        }
    }
}
