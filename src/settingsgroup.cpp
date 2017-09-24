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

#include "KCL/settingsgroup.h"
#include "KCL/filesystemutils.h"

#include <QCoreApplication>
#include <QVariant>
#include <QMetaProperty>
#include <QSettings>
#include <QCoreApplication>

#ifdef KCL_QTQUICK2
    #include <QQmlProperty>
    #define QDeclarativeProperty QQmlProperty
#else
    #include <QDeclarativeProperty>
#endif

static QString *globalIniFilename = NULL;
static QSettings *globalSettings_ = NULL;
static QSettings::Format globalCustomSettingsFormat_ = QSettings::IniFormat;

QString getGlobalIniFilenameSingleton()
{
    if (!globalIniFilename)
        globalIniFilename = new QString();

    return *globalIniFilename;
}

void setGlobalIniFilenameSingleton(const QString value)
{
    if (!globalIniFilename)
        globalIniFilename = new QString();

    *globalIniFilename = value;
}

SettingsGroup::SettingsGroup(QObject *parent) :
    QObject(parent),
    autoSave_(true),
    autoLoad_(true),
    saveDefaults_(true)
{
    autoSaveTimer_ = new QTimer(this);
    autoSaveTimer_->setInterval(1000);
    autoSaveTimer_->setSingleShot(true);
    connect(autoSaveTimer_, SIGNAL(timeout()), this, SLOT(save()));
}

SettingsGroup::~SettingsGroup()
{
    if (autoSave_)
        save();
}

void SettingsGroup::classBegin()
{
}

void SettingsGroup::componentComplete()
{
    SettingsGroup *sg = qobject_cast<SettingsGroup *>(parent());
    if (sg)
        connect(sg, SIGNAL(groupNameChanged()), this, SLOT(updateFullGroupName()));

    updateFullGroupName();

    setCurrentValuesAsDefaultValues();

    if (autoLoad_)
        load();

    connectToNotifySignals();
}

void SettingsGroup::connectToNotifySignals()
{
    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        QDeclarativeProperty dprop(this, name);
        if (dprop.isValid())
            dprop.connectNotifySignal(this, SLOT(handlePropertyChanged()));
    }
}

void SettingsGroup::handlePropertyChanged()
{
    if (autoSave_)
        autoSaveTimer_->start();
}

bool SettingsGroup::shallSkipProperty(const QString &propName)
{
    return propName == "groupName" ||
           propName == "fullGroupName" ||
           propName == "autoLoad" ||
           propName == "autoSave" ||
           propName == "saveDefaults" ||
           propName == "groups";
}

void SettingsGroup::setCurrentValuesAsDefaultValues()
{
    defaults_.clear();

    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        if (shallSkipProperty(name)) continue;

        QVariant value = prop.read(this);
        defaults_.insert(name, value);
    }
}

void SettingsGroup::resetToDefaultValues()
{
    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        if (shallSkipProperty(name)) continue;

        QVariant value;
        if (defaults_.contains(name))
        {
            value = defaults_.value(name);
            prop.write(this, value);
        
            if (prop.hasNotifySignal())
                prop.notifySignal().invoke(this);
        }
    }
}

void SettingsGroup::setGroupName(const QString &groupName)
{
    if (groupName != groupName_)
    {
        groupName_ = groupName;
        updateFullGroupName();
        emit groupNameChanged();
    }
}

void SettingsGroup::setAutoLoad(bool value)
{
    if (value != autoLoad_)
    {
        autoLoad_ = value;
        emit autoLoadChanged();
    }
}

void SettingsGroup::setAutoSave(bool value)
{
    if (value != autoSave_)
    {
        autoSave_ = value;
        emit autoSaveChanged();
    }
}

void SettingsGroup::setSaveDefaults(bool value)
{
    if (value != saveDefaults_)
    {
        saveDefaults_ = value;
        emit saveDefaultsChanged();
    }
}

QSettings *SettingsGroup::settingsInstance()
{
    if (!globalSettings_)
    {
        if (getGlobalIniFilenameSingleton().isEmpty())
            globalSettings_ = new QSettings(globalCustomSettingsFormat_, QSettings::UserScope, qApp->organizationName(), qApp->applicationName());
        else
            globalSettings_ = new QSettings(getGlobalIniFilenameSingleton(), globalCustomSettingsFormat_);
    }

    return globalSettings_;
}

void SettingsGroup::setGlobalIniFilename(const QString fileName)
{
    if (fileName != getGlobalIniFilenameSingleton())
    {
        if (globalSettings_ && globalSettings_->fileName() != fileName)
        {
            delete globalSettings_;
            globalSettings_ = NULL;
        }

        setGlobalIniFilenameSingleton(fileName);
    }
}

QString SettingsGroup::globalIniFilename()
{
    return getGlobalIniFilenameSingleton();
}

void SettingsGroup::setGlobalCustomSettingsFormat(QSettings::Format format)
{
    if (format != globalCustomSettingsFormat_)
    {
        if (globalSettings_ && globalSettings_->format() != format)
        {
            delete globalSettings_;
            globalSettings_ = NULL;
        }

        globalCustomSettingsFormat_ = format;
    }
}

QSettings::Format SettingsGroup::globalCustomSettingsFormat()
{
    return globalCustomSettingsFormat_;
}


#ifdef KCL_QTQUICK2
QQmlListProperty<SettingsGroup> SettingsGroup::groups()
{
    return QQmlListProperty<SettingsGroup>(this, groups_);
}
#else
QDeclarativeListProperty<SettingsGroup> SettingsGroup::groups()
{
    return QDeclarativeListProperty<SettingsGroup>(this, groups_);
}
#endif

void SettingsGroup::save()
{
    emit aboutToSave();

    QSettings *settings = settingsInstance();
    settings->beginGroup(fullGroupName_);

    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        if (shallSkipProperty(name)) continue;

        QVariant value = prop.read(this);

        if (!saveDefaults_ && defaults_.value(name) == value)
            settings->remove(name);
        else
            settings->setValue(name, value);
    }

    settings->endGroup();

    foreach (SettingsGroup *group, groups_)
        group->save();

    emit saved();
}

void SettingsGroup::load()
{
    emit aboutToLoad();

    QSettings *settings = settingsInstance();
    settings->beginGroup(fullGroupName_);

    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        if (shallSkipProperty(name)) continue;

        QVariant value;
        if (settings->contains(name))
            value = settings->value(name);
        else
            value = defaults_.value(name);

        bool result = prop.write(this, value);
        
        if (prop.hasNotifySignal())
            prop.notifySignal().invoke(this);

        if (!result)
            qWarning("Could not set property %s = %s.", (const char *)name.toUtf8(), (const char *)value.toString().toUtf8());
    }

    settings->endGroup();

    foreach (SettingsGroup *group, groups_)
        group->load();

    emit loaded();
}

void SettingsGroup::updateFullGroupName()
{
    SettingsGroup *sg = qobject_cast<SettingsGroup *>(parent());
    if (sg)
        fullGroupName_ = sg->fullGroupName() + "/" + groupName_;
    else
        fullGroupName_ = groupName_;
}
