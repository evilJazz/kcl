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

#include "KCL/performancedatamanager.h"
#include <QDateTime>

#include "KCL/debug.h"

KCL_USE_NAMESPACE

/* PerformanceDataDetail */

PerformanceDataDetail::PerformanceDataDetail(PerformanceDataAspect *parent, const QString &identifier) :
    QObject(parent),
    identifier(identifier),
    startTimeStamp(0),
    stopTimeStamp(0)
{
}

QVariantMap PerformanceDataDetail::asVariant()
{
    QVariantMap map;

    map.insert("identifier", identifier);
    map.insert("startTimeStamp", startTimeStamp);
    map.insert("stopTimeStamp", stopTimeStamp);
    map.insert("totalRuntime", stopTimeStamp - startTimeStamp);

    return map;
}

/* PerformanceDataAspect */

PerformanceDataAspect::PerformanceDataAspect(PerformanceData *parent, const QString &identifier) :
    parent_(parent),
    identifier_(identifier),
    startTimeStamp_(0),
    endTimeStamp_(0)
{
}

PerformanceDataAspect::~PerformanceDataAspect()
{
    qDeleteAll(details_);
    details_.clear();
    detailHash_.clear();
}

PerformanceDataDetail *PerformanceDataAspect::startDetail(const QString &identifier)
{
    qint64 timeStamp = QDateTime::currentMSecsSinceEpoch();

    if (startTimeStamp_ > timeStamp || startTimeStamp_ == 0)
        startTimeStamp_ = timeStamp;

    QHash<QString, PerformanceDataDetail *>::const_iterator i = detailHash_.find(identifier);

    PerformanceDataDetail *detail;

    if (i == detailHash_.end())
    {
        detail = new PerformanceDataDetail(this, identifier);
        details_.append(detail);
        detailHash_.insert(identifier, detail);
    }
    else
    {
        detail = i.value();
    }

    detail->startTimeStamp = timeStamp;
    return detail;
}

PerformanceDataDetail *PerformanceDataAspect::stopDetail(const QString &identifier)
{
    qint64 timeStamp = QDateTime::currentMSecsSinceEpoch();

    if (!detailHash_.contains(identifier))
        qFatal("Start detail %s before stopping it.", identifier.toUtf8().constData());

    PerformanceDataDetail *detail = detailHash_[identifier];

    detail->stopTimeStamp = timeStamp;
    endTimeStamp_ = qMax(endTimeStamp_, timeStamp);

    return detail;
}

QVariantMap PerformanceDataAspect::asVariant()
{
    QVariantMap aspectStat;

    aspectStat.insert("identifier", identifier_);

    QVariantList performanceStat;

    qint64 totalNetRuntime = 0;

    for (int i = 0; i < details_.length(); ++i)
    {
        PerformanceDataDetail *detail = details_.at(i);
        QVariantMap detailMap = detail->asVariant();
        totalNetRuntime += detail->stopTimeStamp - detail->startTimeStamp;
        performanceStat.append(detailMap);
    }

    aspectStat.insert("details", performanceStat);
    aspectStat.insert("startTimeStamp", startTimeStamp_);
    aspectStat.insert("endTimeStamp", endTimeStamp_);
    aspectStat.insert("totalRuntime", endTimeStamp_ - startTimeStamp_);
    aspectStat.insert("totalNetRuntime", totalNetRuntime);

    return aspectStat;
}

/* PerformanceData */

PerformanceData::PerformanceData(PerformanceDataManager *parent, const QString &name) :
    parent_(parent),
    name_(name)
{

}

PerformanceData::~PerformanceData()
{
    qDeleteAll(aspects_);
    aspects_.clear();
    aspectHash_.clear();
}

PerformanceDataAspect *PerformanceData::aspect(const QString &identifier)
{
    QHash<QString, PerformanceDataAspect *>::const_iterator i = aspectHash_.find(identifier);

    if (i == aspectHash_.end())
    {
        PerformanceDataAspect *aspect = new PerformanceDataAspect(this, identifier);
        aspects_.append(aspect);
        aspectHash_.insert(identifier, aspect);
        return aspect;
    }
    else
        return i.value();
}

void PerformanceData::deleteAspect(const QString &identifier)
{
    PerformanceDataAspect *aspect = aspectHash_.take(identifier);
    if (aspect)
    {
        int index = aspects_.indexOf(aspect);
        if (index > -1)
            aspects_.removeAt(index);
    }
}

void PerformanceData::finish()
{
    parent_->emitDataFinished(this);
}

QVariantMap PerformanceData::asVariant()
{
    QVariantMap output;

    QVariantList aspectList;

    qint64 startTimeStamp = 0;
    qint64 endTimeStamp = 0;
    qint64 totalNetRuntime = 0;

    for (int i = 0; i < aspects_.count(); ++i)
    {
        PerformanceDataAspect *aspect = aspects_.at(i);

        startTimeStamp = !startTimeStamp ? aspect->startTimeStamp() : qMin(startTimeStamp, aspect->startTimeStamp());
        endTimeStamp = qMax(endTimeStamp, aspect->endTimeStamp());

        totalNetRuntime += aspect->endTimeStamp() - aspect->startTimeStamp();

        aspectList.append(aspect->asVariant());
    }

    output.insert("name", name_);
    output.insert("aspects", aspectList);
    output.insert("startTimeStamp", startTimeStamp);
    output.insert("endTimeStamp", endTimeStamp);
    output.insert("totalRuntime", endTimeStamp - startTimeStamp);
    output.insert("totalNetRuntime", totalNetRuntime);

    return output;
}

/* PerformanceDataManager */

static PerformanceDataManager *instance = NULL;

PerformanceDataManager &PerformanceDataManager::singleton()
{
    if (!instance)
        instance = new PerformanceDataManager();

    return *instance;
}

PerformanceDataManager::PerformanceDataManager(QObject *parent) :
    QObject(parent),
    keepData_(true)
{
    DGUARDMETHODTIMED;
}

PerformanceDataManager::~PerformanceDataManager()
{
    DGUARDMETHODTIMED;
    clear();
}

PerformanceData *PerformanceDataManager::addNewPerformanceData(const QString &name)
{
    PerformanceData *perfData = new PerformanceData(this, name);
    perfDataList_.append(perfData);
    perfDataHash_.insert(name, perfData);
    return perfData;
}

PerformanceData *PerformanceDataManager::data(const QString &name)
{
    QHash<QString, PerformanceData *>::const_iterator i = perfDataHash_.find(name);

    if (i == perfDataHash_.end())
        return addNewPerformanceData(name);
    else
        return i.value();
}

void PerformanceDataManager::remove(PerformanceData *data)
{
    int index = perfDataList_.indexOf(data);
    if (index > -1)
        removeAt(index);
}

void PerformanceDataManager::removeAt(int index)
{
    PerformanceData *perfData = perfDataList_.takeAt(index);
    perfDataHash_.remove(perfData->name_);
    delete perfData;
}

void PerformanceDataManager::clear()
{
    qDeleteAll(perfDataList_);
	perfDataHash_.clear();
    perfDataList_.clear();
}

QVariant PerformanceDataManager::asVariant(int index)
{
    if (index == -1)
    {
        QVariantList output;

        for (int i = 0; i < perfDataList_.count(); ++i)
        {
            PerformanceData *data = perfDataList_.at(i);
            output.append(data->asVariant());
        }

        return output;
    }
    else
        return perfDataList_.at(index)->asVariant();
}

void PerformanceDataManager::setKeepData(bool value)
{
    if (value != keepData_)
    {
        keepData_ = value;
        emit keepDataChanged();
    }
}

PerformanceDataDetail *PerformanceDataManager::startDetail(const QString &dataName, const QString &aspectId, const QString &detailId)
{
    return data(dataName)->aspect(aspectId)->startDetail(detailId);
}

PerformanceDataDetail *PerformanceDataManager::stopDetail(const QString &dataName, const QString &aspectId, const QString &detailId)
{
    return data(dataName)->aspect(aspectId)->stopDetail(detailId);
}

void PerformanceDataManager::emitDataFinished(PerformanceData *data)
{
    emit dataFinished(data);

    if (!keepData_)
    {
        perfDataList_.removeAll(data);
        perfDataHash_.remove(data->name_);
        data->deleteLater();
    }
}
