/***************************************************************************
 *   Copyright (C) 2011-2012 Andre Beckedorf                               *
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

#include "settingsgroup.h"
#include <QVariant>
#include <QMetaProperty>
#include <QSettings>
#include <QApplication>
#include <QDeclarativeProperty>

static QString *globalIniFilename = NULL;
static QSettings *globalSettings_ = NULL;

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
    autoSaveTimer_.setInterval(1000);
    autoSaveTimer_.setSingleShot(true);
    connect(&autoSaveTimer_, SIGNAL(timeout()), this, SLOT(save()));
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
        autoSaveTimer_.start();
}

bool SettingsGroup::shallSkipProperty(const QString &propName)
{
    return propName == "groupName" || propName == "autoLoad" || propName == "autoSave";
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
        }
    }
}

void SettingsGroup::setGroupName(const QString &groupName)
{
    if (groupName != groupName_)
        groupName_ = groupName;
}

QSettings *SettingsGroup::settingsInstance()
{
    if (!globalSettings_)
    {
        if (getGlobalIniFilenameSingleton().isEmpty())
            globalSettings_ = new QSettings();
        else
            globalSettings_ = new QSettings(getGlobalIniFilenameSingleton(), QSettings::IniFormat);
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

void SettingsGroup::save()
{
    emit aboutToSave();

    QSettings *settings = settingsInstance();
    settings->beginGroup(groupName_);

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
    settings->sync();

    emit saved();
}

void SettingsGroup::load()
{
    emit aboutToLoad();

    QSettings *settings = settingsInstance();
    settings->beginGroup(groupName_);

    for (int i = metaObject()->propertyOffset(); i < metaObject()->propertyCount(); ++i)
    {
        QMetaProperty prop = metaObject()->property(i);
        QString name = prop.name();

        if (shallSkipProperty(name)) continue;

        QVariant value;
        if (settings->contains(name))
        {
            value = settings->value(name);
            bool result = prop.write(this, value);
            if (!result)
                qWarning("Could not set property %s = %s.", (const char *)name.toUtf8(), (const char *)value.toString().toUtf8());
        }
        else
            value = defaults_.value(name);
    }

    settings->endGroup();

    emit loaded();
}
