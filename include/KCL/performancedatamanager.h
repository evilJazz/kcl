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

#ifndef PERFORMANCEDATAMANAGER_H
#define PERFORMANCEDATAMANAGER_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>
#include <QHash>

class PerformanceDataManager;
class PerformanceData;
class PerformanceDataAspect;

class KCL_EXPORT PerformanceDataDetail : public QObject
{
    Q_OBJECT
public:
    PerformanceDataDetail(PerformanceDataAspect *parent, const QString &identifier);
    virtual ~PerformanceDataDetail();

    QString identifier;
    qint64 startTimeStamp;
    qint64 stopTimeStamp;

    QVariantMap asVariant();
};

class KCL_EXPORT PerformanceDataAspect : public QObject
{
    Q_OBJECT
public:
    PerformanceDataAspect(PerformanceData *parent, const QString &identifier);
    virtual ~PerformanceDataAspect();

    Q_INVOKABLE PerformanceDataDetail *startDetail(const QString &identifier);
    Q_INVOKABLE PerformanceDataDetail *stopDetail(const QString &identifier);

    Q_INVOKABLE qint64 startTimeStamp() const { return startTimeStamp_; }
    Q_INVOKABLE qint64 endTimeStamp() const { return endTimeStamp_; }

    Q_INVOKABLE QVariantMap asVariant();

private:
    PerformanceData *parent_;
    QString identifier_;
    qint64 startTimeStamp_;
    qint64 endTimeStamp_;
    QList<PerformanceDataDetail *> details_;
    QHash<QString, PerformanceDataDetail *> detailHash_;
};

class KCL_EXPORT PerformanceData : public QObject
{
    Q_OBJECT
public:
    PerformanceData(PerformanceDataManager *parent, const QString &name);
    virtual ~PerformanceData();

    Q_INVOKABLE PerformanceDataAspect *aspect(const QString &identifier);
    Q_INVOKABLE void deleteAspect(const QString &identifier);

    Q_INVOKABLE int count() const { return aspects_.count(); }
    Q_INVOKABLE void finish();

    Q_INVOKABLE QVariantMap asVariant();

private:
    friend class PerformanceDataManager;
    PerformanceDataManager *parent_;
    QString name_;
    QList<PerformanceDataAspect *> aspects_;
    QHash<QString, PerformanceDataAspect *> aspectHash_;
};

class KCL_EXPORT PerformanceDataManager : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool keepData READ keepData WRITE setKeepData NOTIFY keepDataChanged)
public:
    explicit PerformanceDataManager(QObject *parent = 0);
    virtual ~PerformanceDataManager();

#define globalPerformanceDataManager PerformanceDataManager::singleton()
    static PerformanceDataManager &singleton();

    Q_INVOKABLE PerformanceData *addNewPerformanceData(const QString &name = QString::null);

    Q_INVOKABLE PerformanceData *data(const QString &name);
    Q_INVOKABLE PerformanceData *at(int index) { return perfDataList_[index]; }
    Q_INVOKABLE void remove(PerformanceData *data);
    Q_INVOKABLE void removeAt(int index);
    Q_INVOKABLE int count() const { return perfDataList_.count(); }
    Q_INVOKABLE void clear();

    Q_INVOKABLE QVariant asVariant(int index = -1);

    bool keepData() const { return keepData_; }
    void setKeepData(bool value);

    // Shortcut methods:
    Q_INVOKABLE PerformanceDataDetail* startDetail(const QString &dataName, const QString &aspectId, const QString &detailId);
    Q_INVOKABLE PerformanceDataDetail* stopDetail(const QString &dataName, const QString &aspectId, const QString &detailId);

signals:
    void dataFinished(PerformanceData *data);
    void keepDataChanged();

protected:
    friend class PerformanceData;
    void emitDataFinished(PerformanceData *data);

public:
    bool keepData_;
    QList<PerformanceData *> perfDataList_;
    QHash<QString, PerformanceData *> perfDataHash_;
};

#define PERFDATA_STARTDETAIL(dataId, aspectId, detailId) globalPerformanceDataManager.startDetail(dataId, aspectId, detailId)
#define PERFDATA_STOPDETAIL(dataId, aspectId, detailId) globalPerformanceDataManager.stopDetail(dataId, aspectId, detailId)

#define cStartUp "Start Up"

#endif // PERFORMANCEDATAMANAGER_H
