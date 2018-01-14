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

#include "KCL/systemutils.h"

#ifdef KCL_GUI
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    #include <QtGui/QGuiApplication>
    #include <QScreen>
#else
    #include <QApplication>
    #include <QDesktopWidget>
#endif
#endif

#ifndef Q_OS_WIN
    #ifdef Q_CC_GNU
    #include <malloc.h>
    #endif
#endif

SystemUtils::SystemUtils(QObject *parent) :
    QObject(parent)
{
}

#ifdef KCL_GUI
QVariantMap SystemUtils::physicalScreenInfo()
{
    QVariantMap result;

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    result.insert("height", qApp->primaryScreen()->availableSize().height());
    result.insert("width", qApp->primaryScreen()->availableSize().width());
    result.insert("heightMM", qApp->primaryScreen()->physicalSize().height());
    result.insert("widthMM", qApp->primaryScreen()->physicalSize().height());
    result.insert("logicalDpiX", qApp->primaryScreen()->logicalDotsPerInchX());
    result.insert("logicalDpiY", qApp->primaryScreen()->logicalDotsPerInchY());
    result.insert("physicalDpiX", qApp->primaryScreen()->physicalDotsPerInchX());
    result.insert("physicalDpiY", qApp->primaryScreen()->physicalDotsPerInchY());
#else
    result.insert("height", QApplication::desktop()->height());
    result.insert("width", QApplication::desktop()->width());
    result.insert("heightMM", QApplication::desktop()->heightMM());
    result.insert("widthMM", QApplication::desktop()->widthMM());
    result.insert("logicalDpiX", QApplication::desktop()->logicalDpiX());
    result.insert("logicalDpiY", QApplication::desktop()->logicalDpiY());
    result.insert("physicalDpiX", QApplication::desktop()->physicalDpiX());
    result.insert("physicalDpiY", QApplication::desktop()->physicalDpiY());
#endif

    return result;
}
#endif

void SystemUtils::trimProcessMemoryUsage()
{
    // Attempt to give our memory back to the OS...
#ifdef Q_OS_WIN
    SetProcessWorkingSetSize(GetCurrentProcess(), (SIZE_T) - 1, (SIZE_T) - 1);
#else
#if defined(Q_CC_GNU) && defined(__GNU_LIBRARY__)
    malloc_trim(0);
#endif
#endif
}
