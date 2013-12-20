import QtQuick 1.1
import KCL 1.0
import Qt.labs.folderlistmodel 1.0

AutoColumn {
    width: 720
    height: 500

    autoSizedChildren: [gridView]

    Text {
        anchors.left: parent.left
        anchors.right: parent.right

        height: 40

        text: "Selected folder:\n" + fsModel.folder

        MouseArea {
            anchors.fill: parent
            onClicked:
            {
                fsModel.folder = nativeDialogs.getExistingDirectory
                (
                    "Please select a directory containing pictures",
                    fsModel.folder,
                    true,
                    true
                );
            }
        }
    }

    FolderListModel {
        id: fsModel
        folder: fsUtils.picturesLocation
    }

    GridView {
        id: gridView
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true

        cacheBuffer: height * 2

        cellHeight: 160
        cellWidth: 160

        //*
        model: fsModel

        delegate:
            Rectangle {
                width: gridView.cellWidth
                height: gridView.cellHeight
                border.color: "black"

                ImageRenderSurface {
                    anchors.fill: parent
                    source: "image://imgcache/" + fsUtils.localPathFromUrl(filePath)
                    opacity: status === ImageRenderSurface.Rendered ? 1 : 0
                    Behavior on opacity { NumberAnimation { duration: 250 } }
                }
            }
        //*/

        /*
        model: 2000

        delegate: ImageRenderSurface {
            width: 160
            height: 160
            source: "image://test/red"

            opacity: status === ImageRenderSurface.Rendered ? 1 : 0
            Behavior on opacity { NumberAnimation { duration: 250 } }
        }
        */
    }
}
