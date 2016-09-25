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

#include "KCL/keyeventfilter.h"

#ifdef KCL_QTQUICK2
    #include <QtGui/QGuiApplication>
    #include <QtGui/QStyleHints>
#else
    #include <QApplication>
#endif

KeyEventFilter::KeyEventFilter(QObject *filteredObject) :
    QObject(filteredObject)
{
}

KeyEventFilter::~KeyEventFilter()
{
}

int KeyEventFilter::inputInterval()
{
#ifdef KCL_QTQUICK2
    return qApp->styleHints()->keyboardInputInterval();
#else
    return qApp->keyboardInputInterval();
#endif
}

QString KeyEventFilter::keyToString(int key)
{
    return QKeySequence(key).toString(QKeySequence::PortableText);
}

int KeyEventFilter::interpretKeyEvent(DeclarativeKeyEvent *event)
{
    int result = event->key();

    if (result == Qt::Key_Control ||
        result == Qt::Key_Shift ||
        result == Qt::Key_Alt ||
        result == Qt::Key_AltGr ||
        result == Qt::Key_Meta)
    {
        result = event->modifiers();
    }
    else
    {
        if (event->modifiers() > 0)
            result += event->modifiers();
    }

    return result;
}

bool KeyEventFilter::injectKeyPressed(int key, int modifiers, const QString &text, bool autorep, ushort count)
{
    DeclarativeKeyEvent qke(QEvent::KeyPress, key, modifiers, text, autorep, count, false);
    emit keyPressed(&qke);
    if (qke.isAccepted())
        return true;
    return false;
}

bool KeyEventFilter::injectKeyReleased(int key, int modifiers, const QString &text, bool autorep, ushort count)
{
    DeclarativeKeyEvent qke(QEvent::KeyRelease, key, modifiers, text, autorep, count, false);
    emit keyReleased(&qke);
    if (qke.isAccepted())
        return true;
    return false;
}

bool KeyEventFilter::sendKeyPressedToFilteredObject(int key, int modifiers, const QString &text, bool autorep, ushort count)
{
    DeclarativeKeyEvent qke(QEvent::KeyPress, key, modifiers, text, autorep, count, false);
    return qApp->sendEvent(parent(), qke.keyEvent());
}

bool KeyEventFilter::sendKeyReleasedToFilteredObject(int key, int modifiers, const QString &text, bool autorep, ushort count)
{
    DeclarativeKeyEvent qke(QEvent::KeyRelease, key, modifiers, text, autorep, count, false);
    return qApp->sendEvent(parent(), qke.keyEvent());
}

bool KeyEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == parent())
    {
        switch (event->type())
        {
            case QEvent::KeyPress:
            {
                DeclarativeKeyEvent qke(*static_cast<QKeyEvent *>(event));
                emit keyPressed(&qke);
                if (qke.isAccepted())
                    return true;
                break;
            }
            case QEvent::KeyRelease:
            {
                DeclarativeKeyEvent qke(*static_cast<QKeyEvent *>(event));
                emit keyReleased(&qke);
                if (qke.isAccepted())
                    return true;
                break;
            }
            default:
            {
            }
        }
    }

    return QObject::eventFilter(watched, event);
}
