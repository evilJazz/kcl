/****************************************************************************
**
** Copyright (C) 2011-2017 Andre Beckedorf
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

#include "KCL/propertychangeobserver.h"

#include <QVariant>
#include <QMetaProperty>
#include <QDynamicPropertyChangeEvent>

#include <QDebug>

#ifdef KCL_QTQUICK2
    #include <QQmlProperty>
    #define QDeclarativeProperty QQmlProperty
#else
    #include <QDeclarativeProperty>
#endif

PropertyChangeObserver::PropertyChangeObserver(QObject *parent) :
    QObject(parent)
{
}

bool PropertyChangeObserver::event(QEvent *e)
{
    if (e->type() == QEvent::DynamicPropertyChange)
    {
        QDynamicPropertyChangeEvent *dpce = static_cast<QDynamicPropertyChangeEvent *>(e);

        //DPRINTF("Dynamic property changed: %s", dpce->propertyName().constData());

        emit propertyChanged(dpce->propertyName());
    }
}

void PropertyChangeObserver::classBegin()
{
}

void PropertyChangeObserver::componentComplete()
{
    connectToNotifySignals();
}

void PropertyChangeObserver::connectToNotifySignals()
{
    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        QDeclarativeProperty dprop(this, name);
        if (dprop.isValid())
            dprop.connectNotifySignal(this, SLOT(handleDeclarativePropertyChanged()));
    }
}

void PropertyChangeObserver::handleDeclarativePropertyChanged()
{
    int index = senderSignalIndex() - metaObject()->methodOffset() + metaObject()->propertyOffset();

    QMetaProperty prop = metaObject()->property(index);
    QString name = prop.name();

    QDeclarativeProperty dprop(this, name);
    if (dprop.isValid())
    {
        //DOP(qDebug() << sender() << senderSignalIndex() << "->" << name << ": " << dprop.read());
        emit propertyChanged(name);
    }
}
