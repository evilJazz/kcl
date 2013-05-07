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

#ifndef SETTINGSGROUP_H
#define SETTINGSGROUP_H

#include <QObject>
#include <QEvent>
#include <QVariantMap>
#include <QTimer>
#include <QDeclarativeParserStatus>
#include <QSettings>

class SettingsGroup : public QObject, public QDeclarativeParserStatus
{
    Q_OBJECT
    Q_PROPERTY(QString groupName READ groupName WRITE setGroupName)
    Q_PROPERTY(bool autoLoad READ autoLoad WRITE setAutoLoad)
    Q_PROPERTY(bool autoSave READ autoSave WRITE setAutoSave)
    Q_PROPERTY(bool saveDefaults READ saveDefaults WRITE setSaveDefaults)
public:
    explicit SettingsGroup(QObject *parent = 0);
    virtual ~SettingsGroup();

    void setGroupName(const QString &groupName);
    QString groupName() const { return groupName_; }

    void setAutoLoad(bool value) { autoLoad_ = value; }
    bool autoLoad() { return autoLoad_; }

    void setAutoSave(bool value) { autoSave_ = value; }
    bool autoSave() { return autoSave_; }

    void setSaveDefaults(bool value) { saveDefaults_ = value; }
    bool saveDefaults() { return saveDefaults_; }

    static QSettings *settingsInstance();
    static void setGlobalIniFilename(const QString fileName);
    static QString globalIniFilename();

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

protected:
    // QDeclarativeParserStatus interface
    void classBegin();
    void componentComplete();

private slots:
    void handlePropertyChanged();

private:
    QString groupName_;
    bool autoSave_;
    bool autoLoad_;
    bool saveDefaults_;
    QVariantMap defaults_;

    QTimer autoSaveTimer_;

    void connectToNotifySignals();
    bool shallSkipProperty(const QString &propName);
};

#endif // SETTINGSGROUP_H
