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

#ifndef SETTINGSGROUP_H
#define SETTINGSGROUP_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QEvent>
#include <QVariantMap>
#include <QTimer>
#include <QDeclarativeListProperty>
#include <QDeclarativeParserStatus>
#include <QSettings>

class KCL_EXPORT SettingsGroup : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_INTERFACES(QDeclarativeParserStatus)
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName NOTIFY groupNameChanged)
    Q_PROPERTY(QString fullGroupName READ fullGroupName NOTIFY groupNameChanged)
    Q_PROPERTY(bool autoLoad READ autoLoad WRITE setAutoLoad NOTIFY autoLoadChanged)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave NOTIFY autoSaveChanged)
    Q_PROPERTY(bool saveDefaults READ saveDefaults WRITE setSaveDefaults NOTIFY saveDefaultsChanged)

    Q_PROPERTY(QDeclarativeListProperty<SettingsGroup> groups READ groups)
    Q_CLASSINFO("DefaultProperty", "groups")

public:
    explicit SettingsGroup(QObject *parent = 0);
    virtual ~SettingsGroup();

    void setGroupName(const QString &groupName);
    QString groupName() const { return groupName_; }

    QString fullGroupName() const { return fullGroupName_; }

    void setAutoLoad(bool value);
    bool autoLoad() const { return autoLoad_; }

    void setAutoSave(bool value);
    bool autoSave() const { return autoSave_; }

    void setSaveDefaults(bool value);
    bool saveDefaults() const { return saveDefaults_; }

    static QSettings *settingsInstance();
    static void setGlobalIniFilename(const QString fileName);
    static QString globalIniFilename();

    QDeclarativeListProperty<SettingsGroup> groups();

public slots:
    void save();
    void load();
    void setCurrentValuesAsDefaultValues();
    void resetToDefaultValues();

signals:
    void aboutToLoad();
    void loaded();
    void aboutToSave();
    void saved();

    void groupNameChanged();
    void autoLoadChanged();
    void autoSaveChanged();
    void saveDefaultsChanged();

protected:
    // QDeclarativeParserStatus interface
    void classBegin();
    void componentComplete();

private slots:
    void updateFullGroupName();
    void handlePropertyChanged();

private:
    QString groupName_;
    QString fullGroupName_;
    bool autoSave_;
    bool autoLoad_;
    bool saveDefaults_;
    QVariantMap defaults_;

    QTimer autoSaveTimer_;

    QList<SettingsGroup *> groups_;

    void connectToNotifySignals();
    bool shallSkipProperty(const QString &propName);
};

#endif // SETTINGSGROUP_H
