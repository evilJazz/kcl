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

#ifndef KEYEVENTFILTER_H
#define KEYEVENTFILTER_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QKeyEvent>

class KCL_EXPORT QDeclarativeKeyEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int key READ key)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    QDeclarativeKeyEvent(QEvent::Type type, int key, Qt::KeyboardModifiers modifiers, const QString &text=QString(), bool autorep=false, ushort count=1)
        : event(type, key, modifiers, text, autorep, count) { event.setAccepted(false); }
    QDeclarativeKeyEvent(const QKeyEvent &ke)
        : event(ke) { event.setAccepted(false); }

    int key() const { return event.key(); }
    QString text() const { return event.text(); }
    int modifiers() const { return event.modifiers(); }
    bool isAutoRepeat() const { return event.isAutoRepeat(); }
    int count() const { return event.count(); }

    bool isAccepted() { return event.isAccepted(); }
    void setAccepted(bool accepted) { event.setAccepted(accepted); }

private:
    QKeyEvent event;
};

class KCL_EXPORT KeyEventFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int inputInterval READ inputInterval CONSTANT)
public:
    explicit KeyEventFilter(QObject *parent = 0);
    virtual ~KeyEventFilter();
    
    int inputInterval() const;

signals:
    void keyPressed(QDeclarativeKeyEvent *event);
    void keyReleased(QDeclarativeKeyEvent *event);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
};

#endif // KEYEVENTFILTER_H
