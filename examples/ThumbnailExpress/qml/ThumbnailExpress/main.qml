import QtQuick 1.1
import KCL 1.0
import Qt.labs.folderlistmodel 1.0

AutoColumn {
    width: 720
    height: 500

    autoSizedChildren: [gridView]

    Row {
        anchors.left: parent.left
        anchors.right: parent.right
        height: 40

        Text {
            text: "Selected folder:\n" + fsModel.folder

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    fsModel.folder = NativeDialogs.getExistingDirectory
                    (
                        "Please select a directory containing pictures",
                        fsModel.folder,
                        true,
                        true
                    );
                }
            }
        }

        Rectangle {
            height: parent.height
            width: 40
            color: "red"

            MouseArea {
                anchors.fill: parent
                onClicked:
                {
                    var maskImage = ImageUtils.load(FsUtils.localPathFromUrl(Qt.resolvedUrl("../../../images/test.png")));
                    var resizedMaskImage = ImageUtils.scaled(maskImage, Qt.size(100, 100));
                    resizedMaskImage = ImageUtils.invert(resizedMaskImage, true);

                    var redImage = ImageUtils.empty(Qt.size(100, 100));
                    redImage = ImageUtils.fill(redImage, "red");

                    var combined = ImageUtils.intensityToAlpha(resizedMaskImage, redImage);
                    combined = ImageUtils.applyAlpha(combined, combined, 90);

                    ImageUtils.save(combined, FsUtils.pathAppend(FsUtils.desktopLocation, "resizedTest.png"));
                }
            }
        }
    }

    FolderListModel {
        id: fsModel
        folder: FsUtils.picturesLocation
    }

    GridView {
        id: gridView
        anchors.left: parent.left
        anchors.right: parent.right
        clip: true

        cacheBuffer: height * 2

        cellHeight: 160
        cellWidth: 160

        /*
        model: [
            "image://base64/iVBORw0KGgoAAAANSUhEUgAAAEoAAABYCAYAAABSzmHhAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAyRpVFh0WE1MOmNvbS5hZG9iZS54bXAAAAAAADw/eHBhY2tldCBiZWdpbj0i77u/IiBpZD0iVzVNME1wQ2VoaUh6cmVTek5UY3prYzlkIj8+IDx4OnhtcG1ldGEgeG1sbnM6eD0iYWRvYmU6bnM6bWV0YS8iIHg6eG1wdGs9IkFkb2JlIFhNUCBDb3JlIDUuMy1jMDExIDY2LjE0NTY2MSwgMjAxMi8wMi8wNi0xNDo1NjoyNyAgICAgICAgIj4gPHJkZjpSREYgeG1sbnM6cmRmPSJodHRwOi8vd3d3LnczLm9yZy8xOTk5LzAyLzIyLXJkZi1zeW50YXgtbnMjIj4gPHJkZjpEZXNjcmlwdGlvbiByZGY6YWJvdXQ9IiIgeG1sbnM6eG1wPSJodHRwOi8vbnMuYWRvYmUuY29tL3hhcC8xLjAvIiB4bWxuczp4bXBNTT0iaHR0cDovL25zLmFkb2JlLmNvbS94YXAvMS4wL21tLyIgeG1sbnM6c3RSZWY9Imh0dHA6Ly9ucy5hZG9iZS5jb20veGFwLzEuMC9zVHlwZS9SZXNvdXJjZVJlZiMiIHhtcDpDcmVhdG9yVG9vbD0iQWRvYmUgUGhvdG9zaG9wIENTNiAoTWFjaW50b3NoKSIgeG1wTU06SW5zdGFuY2VJRD0ieG1wLmlpZDo5MDUyQzlBMEIxRUUxMUUyQjk4NDg5QzNENkVCNzA1QiIgeG1wTU06RG9jdW1lbnRJRD0ieG1wLmRpZDo5MDUyQzlBMUIxRUUxMUUyQjk4NDg5QzNENkVCNzA1QiI+IDx4bXBNTTpEZXJpdmVkRnJvbSBzdFJlZjppbnN0YW5jZUlEPSJ4bXAuaWlkOjkwNTJDOTlFQjFFRTExRTJCOTg0ODlDM0Q2RUI3MDVCIiBzdFJlZjpkb2N1bWVudElEPSJ4bXAuZGlkOjkwNTJDOTlGQjFFRTExRTJCOTg0ODlDM0Q2RUI3MDVCIi8+IDwvcmRmOkRlc2NyaXB0aW9uPiA8L3JkZjpSREY+IDwveDp4bXBtZXRhPiA8P3hwYWNrZXQgZW5kPSJyIj8+23UYnAAACvdJREF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"
        ]

        delegate: Rectangle {
            width: gridView.cellWidth
            height: gridView.cellHeight
            border.color: "black"

            ImageRenderSurface {
                anchors.fill: parent
                source: modelData
                opacity: status === ImageRenderSurface.Rendered ? 1 : 0
                Behavior on opacity { NumberAnimation { duration: 250 } }
            }
        }
        //*/

        //*
        model: fsModel

        delegate:
            Rectangle {
                width: gridView.cellWidth
                height: gridView.cellHeight
                border.color: "black"

                ImageRenderSurface {
                    anchors.fill: parent
                    source: "image://imgcache/" + FsUtils.localPathFromUrl(filePath)
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
