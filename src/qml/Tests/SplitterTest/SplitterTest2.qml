import QtQuick 2.0
import "../../KCL"

Rectangle {
    width: 360
    height: 760

    AutoColumn {
        anchors.fill: parent
        autoSizedChildren: [row1]

        AutoRow {
            id: row1
            autoSizedChildren: [col1, col2, col3]

            Rectangle {
                id: col1
                color: "blue"
                border.color: "black"
                border.width: 5
            }

            Splitter {}

            Rectangle {
                id: col2
                color: "gray"
                border.color: "black"
                border.width: 5
            }

            Splitter {}

            Rectangle {
                id: col3
                color: "brown"
                border.color: "black"
                border.width: 5
            }
        }

        AutoRow {
            height: 100
            autoSizedChildren: [switch1, switch2, switch3]

            MouseArea {
                id: switch1
                onClicked: col1.visible = !col1.visible
            }

            MouseArea {
                id: switch2
                onClicked: col2.visible = !col2.visible
            }

            MouseArea {
                id: switch3
                onClicked: col3.visible = !col3.visible
            }
        }
    }
}
