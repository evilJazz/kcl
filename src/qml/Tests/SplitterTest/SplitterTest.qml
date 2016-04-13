import QtQuick 2.0
import "../../KCL"

Rectangle {
    width: 360
    height: 760

    AutoRow {
        anchors.fill: parent
        autoSizedChildren: [col1, col3]

        AutoColumn {
            id: col1
            height: parent.height

            autoSizedChildren: [rect1, rect3]

            Rectangle {
                id: rect1
                width: parent.width
                color: "green"
                border.color: "black"
                border.width: 5
            }

            Splitter {
                autoAttachToSiblings: false
                itemBefore: rect1
                itemAfter: rect2
            }

            Rectangle {
                id: rect2
                width: parent.width
                height: 100
                color: "red"
                border.color: "black"
                border.width: 5
            }

            Splitter {
                autoAttachToSiblings: false
                itemBefore: rect2
                itemAfter: rect3
            }

            Rectangle {
                id: rect3
                width: parent.width
                color: "blue"
                border.color: "black"
                border.width: 5
                visible: false
            }

            Splitter {
                autoAttachToSiblings: false
                itemBefore: rect3
                itemAfter: rect4
            }

            Rectangle {
                id: rect4
                width: parent.width
                height: 30
                color: "yellow"
                border.color: "black"
                border.width: 5
            }
        }

        Splitter {
            autoAttachToSiblings: false
            itemBefore: col1
            itemAfter: col2
        }

        Rectangle {
            id: col2
            width: 130
            height: parent.height
            color: "gray"
            border.color: "black"
            border.width: 5
        }

        Splitter {
            autoAttachToSiblings: false
            itemBefore: col2
            itemAfter: col3
        }

        Rectangle {
            id: col3
            height: parent.height
            color: "brown"
            border.color: "black"
            border.width: 5
        }
    }
}
