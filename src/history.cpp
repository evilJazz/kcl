/***************************************************************************
 *   Copyright (C) 2011-2013 Andre Beckedorf                               *
 * 			     <evilJazz _AT_ katastrophos _DOT_ net>                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include "KCL/history.h"

#undef DEBUG
#include "KCL/debug.h"

History::History(bool undoMode, QObject *parent) :
    QObject(parent),
    undoMode_(undoMode)
{
    clear();
}

History::~History()
{
    clear();
}

void History::clear()
{
    qDeleteAll(snapshots_);
    snapshots_.clear();
    isLoadingSnapshot_ = false;
    activeIndex_ = -1;
    emit changed();
    emit activeIndexChanged();
}

bool History::canGoBack() const
{
    DGUARDMETHOD;
    DPRINTF("activeIndex_: %d", activeIndex_);

    if (undoMode_)
        return (activeIndex_ > -1) && (snapshots_.count() > 0) && !isLoadingSnapshot_;
    else
        return (activeIndex_ > 0) && (snapshots_.count() > 0) && !isLoadingSnapshot_;
}

bool History::canGoForward() const
{
    DGUARDMETHOD;
    DPRINTF("activeIndex_: %d", activeIndex_);
    return (snapshots_.count() - (activeIndex_ + 1) > 0) && (snapshots_.count() > 0) && !isLoadingSnapshot_;
}

bool History::replaceSnapshotWithNew(HistorySnapshot *newSnapshot)
{
    DGUARDMETHOD;

    if (activeIndex_ > -1)
    {
        DPRINTF("Saving current state to active snapshot %d, total snapshot count: %d", activeIndex_, snapshots_.count());
        delete snapshots_[activeIndex_];
        snapshots_[activeIndex_] = newSnapshot;

        dumpSnapshot(activeIndex_);

        emit changed();

        return true;
    }
    else
    {
        DPRINTF("Adding new snapshot because the list is empty...");
        return addSnapshot(newSnapshot);
    }
}

bool History::addSnapshot(HistorySnapshot *newSnapshot, bool forceEvenIfEqual)
{
    DGUARDMETHOD;

    DPRINTF("activeIndex_: %d", activeIndex_);

    if (!isLoadingSnapshot_)
    {
        bool shallAdd = true;

        if (!forceEvenIfEqual && activeIndex_ > -1 && *(snapshots_.at(activeIndex_)) == *newSnapshot)
        {
            DPRINTF("Found similar snapshot, will not add.");
            shallAdd = false;
        }

        if (forceEvenIfEqual || shallAdd)
        {
            if (canGoForward())
            {
                int index = snapshots_.count() - 1;
                while (index > activeIndex_)
                {
                    delete snapshots_.last();
                    snapshots_.removeLast();
                    --index;
                }
            }

            snapshots_.append(newSnapshot);
            activeIndex_ = snapshots_.count() - 1;
            dumpSnapshot(activeIndex_);
            emit changed();
            emit activeIndexChanged();

            return true;
        }
    }

    delete newSnapshot;
    return false;
}

void History::loadSnapshot(int index)
{
    DGUARDMETHOD;

    isLoadingSnapshot_ = true;

    dumpSnapshot(index);

    HistorySnapshot *snapshot = snapshots_.at(index);
    snapshot->restore();

    emit changed();

    isLoadingSnapshot_ = false;
}

void History::dumpSnapshot(int index)
{
    DGUARDMETHOD;
    DPRINTF("Snapshot %d:", index);

    HistorySnapshot *snapshot = snapshots_.at(index);
    // TODO...
}

void History::goBack() // UNDO
{
    DGUARDMETHOD;
    if (canGoBack())
    {
        emit aboutToGoBack();

        if (undoMode_)
        {
            loadSnapshot(activeIndex_);
            --activeIndex_;
        }
        else
        {
            --activeIndex_;
            loadSnapshot(activeIndex_);
        }

        emit activeIndexChanged();
    }
}

void History::goForward() // REDO
{
    DGUARDMETHOD;
    if (canGoForward())
    {
        emit aboutToGoForward();
        ++activeIndex_;
        loadSnapshot(activeIndex_);

        emit activeIndexChanged();
    }
}
