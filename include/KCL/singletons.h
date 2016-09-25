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

#ifndef SINGLETONS_H
#define SINGLETONS_H

#include "kcl_global.h"

#include <QList>
#include <QGlobalStatic>

typedef void * (*SingletonInitializerFunc)();

class KCL_EXPORT SingletonRegistry
{
public:
    static SingletonRegistry &instance();
    #define Singletons SingletonRegistry::instance()

    void registerInitializer(SingletonInitializerFunc myFunction);

    void initialize();

private:
    QList<SingletonInitializerFunc> initializers_;
};

#define KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY(REGISTRY, TYPE, NAME, ARGS) \
    Q_GLOBAL_STATIC_WITH_ARGS(TYPE, NAME, ARGS) \
    struct SingletonInitializer_ ##NAME \
    { \
        SingletonInitializer_ ##NAME() { REGISTRY->registerInitializer(&SingletonInitializer_ ##NAME::initializer); } \
        static void *initializer() { return NAME(); } \
    }; \
    static SingletonInitializer_ ##NAME singletonInitializer_ ##NAME;

#define KCL_SINGLETON_REGISTER_AT_REGISTRY(REGISTRY, TYPE, NAME) KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY(REGISTRY, TYPE, NAME, ())

#define KCL_SINGLETON_WITH_ARGS_REGISTER(TYPE, NAME, ARGS) KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY((&SingletonRegistry::instance()), TYPE, NAME, ARGS)
#define KCL_SINGLETON_REGISTER(TYPE, NAME) KCL_SINGLETON_WITH_ARGS_REGISTER_AT_REGISTRY((&SingletonRegistry::instance()), TYPE, NAME, ())

#endif // SINGLETONS_H
