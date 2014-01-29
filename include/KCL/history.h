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

#ifndef HISTORY_H
#define HISTORY_H

#include "KCL/kcl_global.h"

#include <QObject>

namespace KCL {

class KCL_EXPORT HistorySnapshot
{
public:
    virtual ~HistorySnapshot() {}
    virtual bool operator==(const HistorySnapshot &other) const = 0;
    virtual void restore() = 0;
};

class KCL_EXPORT History : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool canGoBack READ canGoBack NOTIFY activeIndexChanged)
    Q_PROPERTY(bool canGoForward READ canGoForward NOTIFY activeIndexChanged)
    Q_PROPERTY(bool isLoadingSnapshot READ isLoadingSnapshot NOTIFY isLoadingSnapshotChanged)
public:
    explicit History(bool undoMode = false, QObject *parent = 0);
    virtual ~History();

    void clear();

    bool canGoBack() const;
    bool canGoForward() const;

    bool isLoadingSnapshot() const { return isLoadingSnapshot_; }

    Q_INVOKABLE bool addSnapshot(HistorySnapshot *newSnapshot, bool forceEvenIfEqual = false);
    Q_INVOKABLE bool replaceSnapshotWithNew(HistorySnapshot *newSnapshot);

signals:
    void changed();
    void activeIndexChanged();
    void isLoadingSnapshotChanged();

    void aboutToGoBack();
    void aboutToGoForward();

public slots:
    void goBack();
    void goForward();

private:
    bool undoMode_;
    int activeIndex_;
    bool isLoadingSnapshot_;
    QList<HistorySnapshot *> snapshots_;

    void loadSnapshot(int index);
    void dumpSnapshot(int index);
};

}

#endif // HISTORY_H
