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

#include "kclplugin.h"

#ifdef KCL_QTQUICK2
    #include <QtQuick>
    #define QDeclarativeEngine QQmlEngine
#else
    #include <QtDeclarative>
#endif

#include "KCL/settingsgroup.h"
#include "KCL/filesystemutils.h"
#include "KCL/cursorarea.h"
#include "KCL/imagerendersurface.h"
#include "KCL/filescanner.h"
#include "KCL/binaryfiledownloader.h"
#include "KCL/performancedatamanager.h"
#include "KCL/progressmanager.h"
#include "KCL/keyeventfilter.h"
#include "KCL/settingsgroup.h"

#ifdef KCL_WIDGETS
    #include "KCL/nativedialogs.h"
    #include "KCL/logging.h"
#endif

#ifndef KCL_QTQUICK2
    #include "KCL/declarativedebug.h"
#endif

void KCLPlugin::registerTypes(const char *uri)
{
    //@uri KCL
    qmlRegisterType<BinaryFileDownloader>(uri, 1, 0, "BinaryFileDownloader");
    qmlRegisterType<SettingsGroup>(uri, 1, 0, "SettingsGroup");
    qmlRegisterType<FileScanner>(uri, 1, 0, "FileScanner");
    qmlRegisterType<CursorArea>(uri, 1, 0, "CursorArea");
    qmlRegisterType<ImageRenderSurface>(uri, 1, 0, "ImageRenderSurface");

#ifdef KCL_WIDGETS
    qmlRegisterType<NativeDialogs>(uri, 1, 0, "NativeDialogs");
#endif

    qmlRegisterType<QTimer>(uri, 1, 0, "QtTimer");

    qmlRegisterUncreatableType<FileSystemUtils>(uri, 1, 0, "FileSystemUtils", "Can not instantiate FileSystemUtils, use fsUtils singleton instead.");

    qmlRegisterUncreatableType<PerformanceDataManager>(uri, 1, 0, "PerformanceDataManager", "Can not instantiate PerformanceDataManager, use performanceDataManager singleton instead.");
    qmlRegisterType<PerformanceData>();
    qmlRegisterType<PerformanceDataAspect>();
    qmlRegisterType<PerformanceDataDetail>();

    qmlRegisterUncreatableType<ProgressManager>(uri, 1, 0, "ProgressManager", "Can not instantiate ProgressManager, use progressManager singleton instead.");
    qmlRegisterType<ProgressContext>();

    qmlRegisterType<QDeclarativeKeyEvent>();
    qmlRegisterType<KeyEventFilter>();

#ifndef KCL_QTQUICK2
    qmlRegisterType<DeclarativeDebug>();
#endif
}

void KCLPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImportPath(":/"); // to allow "import KCL 1.0"

    FileSystemUtils *fsUtils = new FileSystemUtils(engine);
    engine->rootContext()->setContextProperty("fsUtils", fsUtils);

#ifdef KCL_WIDGETS
    NativeDialogs *nativeDialogs = new NativeDialogs(engine);
    engine->rootContext()->setContextProperty("nativeDialogs", nativeDialogs);
#endif

#ifndef KCL_QTQUICK2
    DeclarativeDebug *declarativeDebug = new DeclarativeDebug(engine);
    engine->rootContext()->setContextProperty("debug", declarativeDebug);
#endif

    engine->rootContext()->setContextProperty("performanceDataManager", &qPerformanceDataManager);
    engine->rootContext()->setContextProperty("progressManager", &globalProgressManager);
}

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(kclplugin, KCLPlugin)
#endif
