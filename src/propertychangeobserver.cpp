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

PropertyChangeObserver::~PropertyChangeObserver()
{
}

QStringList PropertyChangeObserver::ignoredPropertyNames() const
{
    return ignoredPropertyNames_.values();
}

void PropertyChangeObserver::setIgnoredPropertyNames(const QStringList &ignoredPropertyNames)
{
    if (ignoredPropertyNames_.values() != ignoredPropertyNames)
    {
        ignoredPropertyNames_ = QSet<QString>::fromList(ignoredPropertyNames);
        emit ignoredPropertyNamesChanged();
    }
}

bool PropertyChangeObserver::hasOwnProperty(const QString &propertyName)
{
    QDeclarativeProperty dprop(this, propertyName);
    if (dprop.isValid())
        return true;

    return this->property(propertyName.toLatin1().constData()).isValid();
}

QVariant PropertyChangeObserver::getProperty(const QString &propertyName)
{
    QVariant result;

    QDeclarativeProperty dprop(this, propertyName);
    if (dprop.isValid())
        result = dprop.read();

    if (!result.isValid())
        result = this->property(propertyName.toLatin1().constData());

    return result;
}

bool PropertyChangeObserver::event(QEvent *e)
{
    if (e->type() == QEvent::DynamicPropertyChange)
    {
        QDynamicPropertyChangeEvent *dpce = static_cast<QDynamicPropertyChangeEvent *>(e);

        //DPRINTF("Dynamic property changed: %s", dpce->propertyName().constData());

        emitPropertyChanged(dpce->propertyName());
    }
    else if (e->type() == QEvent::ParentChange)
    {
        emit parentChanged();
    }

    return QObject::event(e);
}

void PropertyChangeObserver::classBegin()
{
}

void PropertyChangeObserver::componentComplete()
{
    emit parentChanged();
    connectToNotifySignals();
}

void PropertyChangeObserver::connectToNotifySignals()
{
    for (int i = 0; i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        QDeclarativeProperty dprop(this, name);
        if (dprop.isValid())
        {
            //qDebug() << "Connecting to signal" << name;
            dprop.connectNotifySignal(this, SLOT(handleDeclarativePropertyChanged()));
        }
    }
}

void PropertyChangeObserver::emitPropertyChanged(const QString &propertyName)
{
    if (!ignoredPropertyNames_.contains(propertyName))
        emit propertyChanged(propertyName);
}

void PropertyChangeObserver::handleDeclarativePropertyChanged()
{
    int signalIndex = senderSignalIndex();

    for (int i = 0; i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);

        if (prop.notifySignalIndex() == signalIndex)
        {
            QString name = prop.name();
            QDeclarativeProperty dprop(this, name);
            if (dprop.isValid())
            {
                //DOP(qDebug() << sender() << senderSignalIndex() << "->" << name << ": " << dprop.read());
                emitPropertyChanged(name);
                return;
            }
        }
    }
}
