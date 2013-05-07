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

#include <QtDeclarative>

#include "KCL/settingsgroup.h"
#include "KCL/filesystemutils.h"
#include "KCL/nativedialogs.h"
#include "KCL/cursorarea.h"
#include "KCL/filescanner.h"
#include "KCL/binaryfiledownloader.h"
#include "KCL/performancedatamanager.h"
#include "KCL/progressmanager.h"
#include "KCL/keyeventfilter.h"
#include "KCL/settingsgroup.h"
#include "KCL/logging.h"

void KCLPlugin::registerTypes(const char *uri)
{
    //@uri KCL
    qmlRegisterType<BinaryFileDownloader>(uri, 1, 0, "BinaryFileDownloader");
    qmlRegisterType<SettingsGroup>(uri, 1, 0, "SettingsGroup");
    qmlRegisterType<FileScanner>(uri, 1, 0, "FileScanner");
    qmlRegisterType<NativeDialogs>(uri, 1, 0, "NativeDialogs");
    qmlRegisterType<CursorArea>(uri, 1, 0, "CursorArea");

    qmlRegisterUncreatableType<FileSystemUtils>(uri, 1, 0, "FileSystemUtils", "Can not instantiate FileSystemUtils, use fsUtils singleton instead.");

    qmlRegisterUncreatableType<PerformanceDataManager>(uri, 1, 0, "PerformanceDataManager", "Can not instantiate PerformanceDataManager, use performanceDataManager singleton instead.");
    qmlRegisterType<PerformanceData>();
    qmlRegisterType<PerformanceDataAspect>();

    qmlRegisterUncreatableType<ProgressManager>(uri, 1, 0, "ProgressManager", "Can not instantiate ProgressManager, use progressManager singleton instead.");
    qmlRegisterType<ProgressContext>();

    qmlRegisterType<QDeclarativeKeyEvent>();
    qmlRegisterType<KeyEventFilter>();
}

void KCLPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImportPath(":/"); // to allow "import KCL 1.0"

    FileSystemUtils *fsUtils = new FileSystemUtils(engine);
    engine->rootContext()->setContextProperty("fsUtils", fsUtils);

    NativeDialogs *nativeDialogs = new NativeDialogs(engine);
    engine->rootContext()->setContextProperty("nativeDialogs", nativeDialogs);

    engine->rootContext()->setContextProperty("performanceDataManager", &qPerformanceDataManager);
    engine->rootContext()->setContextProperty("progressManager", &globalProgressManager);
}

Q_EXPORT_PLUGIN2(styleplugin, KCLPlugin)
