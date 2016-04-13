import QtQuick 2.0

ListView {
    id: container

    property int heightOfSectionDelegate: 70
    property int heightOfDelegate: 50

    pixelAligned: true //@QtQuick2

    function firstVisibleItem()
    {
        if (count == 0)
            return -1;

        var index = indexAt(contentX + 5, contentY + 1);

        if (index == -1)
            index = indexAt(contentX + 5,  contentY + 1 + heightOfDelegate);

        if (index == -1)
            index = indexAt(contentX + 5,  contentY + 1 + heightOfSectionDelegate);

        return index;
    }

    function lastVisibleItem()
    {
        if (count == 0)
            return -1;

        var index = indexAt(contentX, contentY + height - 1);

        if (index == -1)
            index = indexAt(contentX, contentY + height - 1 - heightOfDelegate);

        if (index == -1)
            index = indexAt(contentX, contentY + height - 1 - heightOfSectionDelegate);

        return index;
    }

    function itemsPerPage()
    {
        return Math.max(1, lastVisibleItem() - firstVisibleItem());
        //return Math.max(1, Math.ceil(height / heightOfDelegate));
    }

    function isItemVisible(index)
    {
        return (index >= firstVisibleItem()) && (index <= lastVisibleItem());
    }

    function animatedPositionViewAtIndex(index, type)
    {
        var pos = contentY;
        positionViewAtIndex(index, type);
        var destPos = contentY;

        anim.from = pos;
        anim.to = destPos;
        anim.running = true;
    }

    NumberAnimation { id: anim; target: container; property: "contentY"; duration: 500 }

    signal currentItemSelectedByUser()
    signal currentItemActivated()
    signal currentItemShowContextMenu(variant data)
    signal setCurrentIndex(int index)

    Keys.onPressed: {
        // with the ListView !interactive (non-flicking) we have to handle arrow keys
        if (event.key == Qt.Key_Up)
        {
            if (!currentItem)
                setCurrentIndex(lastVisibleItem());
            else if (currentIndex > 0)
                setCurrentIndex(currentIndex - 1);

            event.accepted = true;
        }
        else if (event.key == Qt.Key_Down)
        {
            if (!currentItem)
                setCurrentIndex(firstVisibleItem());
            else if (currentIndex + 1 < model.count)
                setCurrentIndex(currentIndex + 1);

            event.accepted = true;
        }
        else if (event.key == Qt.Key_PageUp)
        {
            if (!currentItem)
                setCurrentIndex(lastVisibleItem());
            else
                setCurrentIndex(Math.max(currentIndex - itemsPerPage(), 0));

            event.accepted = true;
        }
        else if (event.key == Qt.Key_PageDown)
        {
            if (!currentItem)
                setCurrentIndex(firstVisibleItem());
            else
                setCurrentIndex(Math.min(currentIndex + itemsPerPage(), model.count - 1));

            event.accepted = true;
        }
        else if (event.key == Qt.Key_Home)
        {
            setCurrentIndex(0);
            event.accepted = true;
        }
        else if (event.key == Qt.Key_End)
        {
            setCurrentIndex(model.count - 1);
            event.accepted = true;
        }
        else if (event.key == Qt.Key_Enter || event.key == Qt.Key_Return)
        {
            currentItemActivated();
            event.accepted = true;
        }
    }
}
