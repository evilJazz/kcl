/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

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
    signal itemClickedWithModifiers(int index, int modifiers)
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
