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

#include "KCL/keyeventfilter.h"

#ifdef KCL_QTQUICK2
    #include <QtGui/QGuiApplication>
    #include <QtGui/QStyleHints>
#else
    #include <QApplication>
#endif

KeyEventFilter::KeyEventFilter(QObject *parent) :
    QObject(parent)
{
}

KeyEventFilter::~KeyEventFilter()
{
}

int KeyEventFilter::inputInterval() const
{
#ifdef KCL_QTQUICK2
    return qApp->styleHints()->keyboardInputInterval();
#else
    return qApp->keyboardInputInterval();
#endif
}

bool KeyEventFilter::injectKeyPressed(int key, Qt::KeyboardModifiers modifiers, const QString &text, bool autorep, ushort count)
{
    DeclarativeKeyEvent qke(QEvent::KeyPress, key, modifiers, text, autorep, count);
    emit keyPressed(&qke);
    if (qke.isAccepted())
        return true;
    return false;
}

bool KeyEventFilter::injectKeyReleased(int key, Qt::KeyboardModifiers modifiers, const QString &text, bool autorep, ushort count)
{
    DeclarativeKeyEvent qke(QEvent::KeyRelease, key, modifiers, text, autorep, count);
    emit keyReleased(&qke);
    if (qke.isAccepted())
        return true;
    return false;
}

bool KeyEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        DeclarativeKeyEvent qke(*static_cast<QKeyEvent *>(event));
        emit keyPressed(&qke);
        if (qke.isAccepted())
            return true;
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        DeclarativeKeyEvent qke(*static_cast<QKeyEvent *>(event));
        emit keyReleased(&qke);
        if (qke.isAccepted())
            return true;
    }

    return QObject::eventFilter(watched, event);
}
