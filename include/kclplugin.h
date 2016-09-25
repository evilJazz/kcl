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

#ifndef KCLPLUGIN_H
#define KCLPLUGIN_H

#include "KCL/kcl_global.h"

#ifdef KCL_QTQUICK2
    #include <QQmlExtensionPlugin>
#else
    #include <QDeclarativeExtensionPlugin>
#endif

class KCL_EXPORT KCLPlugin :
#ifdef KCL_QTQUICK2
    public QQmlExtensionPlugin
#else
    public QDeclarativeExtensionPlugin
#endif
{
    Q_OBJECT
#if defined(KCL_PLUGIN) && QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    Q_PLUGIN_METADATA(IID "net.katastrophos.kcl")
#endif
public:
    void registerTypes(const char *uri);
#ifdef KCL_QTQUICK2
    void initializeEngine(QQmlEngine *engine, const char *uri);
#else
    void initializeEngine(QDeclarativeEngine *engine, const char *uri);
#endif
};

#endif // KCLPLUGIN_H
