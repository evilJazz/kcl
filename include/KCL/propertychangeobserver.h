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

#ifndef PROPERTYCHANGEOBSERVER_H
#define PROPERTYCHANGEOBSERVER_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QEvent>
#include <QSet>
#include <QStringList>

#ifdef KCL_QTQUICK2
    #include <QtQuick>
#else
    #include <QDeclarativeListProperty>
    #include <QDeclarativeParserStatus>
#endif

class KCL_EXPORT PropertyChangeObserver :
    public QObject,
#ifdef KCL_QTQUICK2
    public QQmlParserStatus
#else
    public QDeclarativeParserStatus
#endif
{
    Q_OBJECT
    Q_PROPERTY(QObject *parent READ parent WRITE setParent NOTIFY parentChanged)
    Q_PROPERTY(QStringList ignoredPropertyNames READ ignoredPropertyNames WRITE setIgnoredPropertyNames NOTIFY ignoredPropertyNamesChanged)
#ifdef KCL_QTQUICK2
    Q_INTERFACES(QQmlParserStatus)
#else
    Q_INTERFACES(QDeclarativeParserStatus)
#endif
public:
    explicit PropertyChangeObserver(QObject *parent = 0);
    virtual ~PropertyChangeObserver();

    QStringList ignoredPropertyNames() const;
    void setIgnoredPropertyNames(const QStringList &ignoredPropertyNames);

    bool hasOwnProperty(const QString &propertyName);
    QVariant getProperty(const QString &propertyName);

public slots:

signals:
    void propertyChanged(const QString &propertyName);
    void parentChanged();
    void ignoredPropertyNamesChanged();

protected slots:
    void handleDeclarativePropertyChanged();

protected:
    QSet<QString> ignoredPropertyNames_;

    bool event(QEvent *e);

    // QDeclarativeParserStatus interface
    void classBegin();
    void componentComplete();

    void connectToNotifySignals();
    void emitPropertyChanged(const QString &propertyName);
};

#endif // PROPERTYCHANGEOBSERVER_H
