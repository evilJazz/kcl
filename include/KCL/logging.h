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

#ifndef LOGGING_H
#define LOGGING_H

#include "KCL/kcl_global.h"

#include <QObject>

class KCL_EXPORT Logging: public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool logWindowEnabled READ isLogWindowEnabled NOTIFY logWindowEnabledChanged)
public:
    static Logging& singleton();
#	define globalLogging Logging::singleton()

    Q_INVOKABLE void registerHandler();
    Q_INVOKABLE void unregisterHandler();

    bool isLogWindowEnabled() const;

    Q_INVOKABLE void enableLogFile(const QString &fileName);
    Q_INVOKABLE void disableLogFile();

    Q_INVOKABLE bool isEnabled() const;
    Q_INVOKABLE QString logFileName() const;

signals:
    void logWindowEnabledChanged();

public slots:
#ifdef KCL_WIDGETS
    void enableLogWindow();
    void disableLogWindow();
#endif

protected:
    explicit Logging();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &);
#else
    static void customMessageHandler(QtMsgType type, const char *msg);
#endif

    static void kaCustomMessageHandler(const QString &msg);
};

#endif // LOGGING_H
