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

class KCL_EXPORT DeclarativeKeyEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int key READ key)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat)
    Q_PROPERTY(bool isSpontaneous READ spontaneous)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)
    Q_PROPERTY(quint32 nativeScanCode READ nativeScanCode)
    Q_PROPERTY(quint32 nativeVirtualKey READ nativeVirtualKey)
    Q_PROPERTY(quint32 nativeModifiers READ nativeModifiers)
public:
    DeclarativeKeyEvent(QEvent::Type type, int key, int modifiers, const QString &text=QString(), bool autorep=false, ushort count=1, bool spontaneous = true)
        : event(type, key, (Qt::KeyboardModifiers)modifiers, text, autorep, count), spontaneous_(spontaneous) { event.setAccepted(false); }
    DeclarativeKeyEvent(const QKeyEvent &ke)
        : event(ke) { spontaneous_ = event.spontaneous(); event.setAccepted(false); }

    int key() const { return event.key(); }
    QString text() const { return event.text(); }
    int modifiers() const { return event.modifiers(); }
    bool isAutoRepeat() const { return event.isAutoRepeat(); }
    int count() const { return event.count(); }

    bool spontaneous() { return spontaneous_; }

    bool isAccepted() { return event.isAccepted(); }
    void setAccepted(bool accepted) { event.setAccepted(accepted); }

    quint32 nativeScanCode() const { return event.nativeScanCode(); }
    quint32 nativeVirtualKey() const { return event.nativeVirtualKey(); }
    quint32 nativeModifiers() const { return event.nativeModifiers(); }

private:
    QKeyEvent event;
    bool spontaneous_;
};

class KCL_EXPORT KeyEventFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int inputInterval READ inputInterval CONSTANT)
public:
    explicit KeyEventFilter(QObject *parent = 0);
    virtual ~KeyEventFilter();
    
    Q_INVOKABLE static int inputInterval();

    Q_INVOKABLE static QString keyToString(int key);
    Q_INVOKABLE static int interpretKeyEvent(DeclarativeKeyEvent *event);

    Q_INVOKABLE bool injectKeyPressed(int key, int modifiers = 0, const QString &text = QString(), bool autorep = false, ushort count = 1);
    Q_INVOKABLE bool injectKeyReleased(int key, int modifiers = 0, const QString &text = QString(), bool autorep = false, ushort count = 1);

signals:
    void keyPressed(DeclarativeKeyEvent *event);
    void keyReleased(DeclarativeKeyEvent *event);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
};

#endif // KEYEVENTFILTER_H
