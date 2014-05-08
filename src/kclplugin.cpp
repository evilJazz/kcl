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

#ifdef KCL_filesystemutils
    #include "KCL/filesystemutils.h"
#endif

#ifdef KCL_objectutils
    #include "KCL/objectutils.h"
#endif

#ifdef KCL_cursorarea
    #include "KCL/cursorarea.h"
#endif

#ifdef KCL_sceneutils
    #include "KCL/sceneutils.h"
#endif

#ifdef KCL_networkutils
    #include "KCL/networkutils.h"
#endif

#ifdef KCL_imagerendersurface
    #include "KCL/imagerendersurface.h"
#endif

#ifdef KCL_filescanner
    #include "KCL/filescanner.h"
#endif

#ifdef KCL_binaryfiledownloader
    #include "KCL/binaryfiledownloader.h"
#endif

#ifdef KCL_performancedatamanager
    #include "KCL/performancedatamanager.h"
#endif

#ifdef KCL_progressmanager
    #include "KCL/progressmanager.h"
#endif

#ifdef KCL_keyeventfilter
    #include "KCL/keyeventfilter.h"
#endif

#ifdef KCL_settingsgroup
    #include "KCL/settingsgroup.h"
#endif

#ifdef KCL_colorutils
    #include "KCL/colorutils.h"
#endif

#ifdef KCL_graphicsutils
    #include "KCL/graphicsutils.h"
#endif

#ifdef KCL_imageutils
    #include "KCL/imageutils.h"
#endif

#ifdef KCL_datetimeutils
    #include "KCL/datetimeutils.h"
#endif

#ifdef KCL_nativedialogs
    #include "KCL/nativedialogs.h"
#endif

#ifdef KCL_logging
    #include "KCL/logging.h"
#endif

#ifdef KCL_declarativedebug
    #include "KCL/declarativedebug.h"
#endif

#ifdef KCL_base64imageprovider
    #include "KCL/base64imageprovider.h"
#endif

void KCLPlugin::registerTypes(const char *uri)
{
    //@uri KCL

#ifdef KCL_binaryfiledownloader
    qmlRegisterType<BinaryFileDownloader>(uri, 1, 0, "BinaryFileDownloader");
#endif

#ifdef KCL_settingsgroup
    qmlRegisterType<SettingsGroup>(uri, 1, 0, "SettingsGroup");
#endif

#ifdef KCL_filescanner
    qmlRegisterType<FileScanner>(uri, 1, 0, "FileScanner");
#endif

#ifdef KCL_cursorarea
    qmlRegisterType<CursorArea>(uri, 1, 0, "CursorArea");
#endif

#ifdef KCL_imagerendersurface
    qmlRegisterType<ImageRenderSurface>(uri, 1, 0, "ImageRenderSurface");
#endif

    qmlRegisterType<QTimer>(uri, 1, 0, "QtTimer");

#ifdef KCL_performancedatamanager
    qmlRegisterType<PerformanceData>();
    qmlRegisterType<PerformanceDataAspect>();
    qmlRegisterType<PerformanceDataDetail>();
#endif

#ifdef KCL_progressmanager
    qmlRegisterType<ProgressContext>();
#endif

#ifdef KCL_keyeventfilter
    qmlRegisterType<DeclarativeKeyEvent>();
    qmlRegisterType<KeyEventFilter>();
#endif

#ifdef KCL_declarativedebug
    qmlRegisterType<DeclarativeDebug>();
#endif
}

void KCLPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImportPath(":/"); // to allow "import KCL 1.0"

#ifdef KCL_filesystemutils
    FileSystemUtils *fsUtils = new FileSystemUtils(engine);
    engine->rootContext()->setContextProperty("FsUtils", fsUtils);
#endif

#ifdef KCL_objectutils
    ObjectUtils *objectUtils = new ObjectUtils(engine);
    engine->rootContext()->setContextProperty("ObjectUtils", objectUtils);
#endif

#ifdef KCL_colorutils
    ColorUtils *colorUtils = new ColorUtils(engine);
    engine->rootContext()->setContextProperty("ColorUtils", colorUtils);
#endif

#ifdef KCL_graphicsutils
    GraphicsUtils *graphicsUtils = new GraphicsUtils(engine);
    engine->rootContext()->setContextProperty("GraphicsUtils", graphicsUtils);
#endif

#ifdef KCL_imageutils
    ImageUtils *imageUtils = new ImageUtils(engine);
    engine->rootContext()->setContextProperty("ImageUtils", imageUtils);
#endif

#ifdef KCL_sceneutils
    SceneUtils *sceneUtils = new SceneUtils(engine);
    engine->rootContext()->setContextProperty("SceneUtils", sceneUtils);
#endif

#ifdef KCL_datetimeutils
    DateTimeUtils *dateTimeUtils = new DateTimeUtils(engine);
    engine->rootContext()->setContextProperty("DateTimeUtils", dateTimeUtils);
#endif

#ifdef KCL_networkutils
    NetworkUtils *networkUtils = new NetworkUtils(engine);
    engine->rootContext()->setContextProperty("NetworkUtils", networkUtils);
#endif

#ifdef KCL_nativedialogs
    NativeDialogs *nativeDialogs = new NativeDialogs(engine);
    engine->rootContext()->setContextProperty("NativeDialogs", nativeDialogs);
#endif

#ifdef KCL_declarativedebug
    DeclarativeDebug *declarativeDebug = new DeclarativeDebug(engine);
    engine->rootContext()->setContextProperty("Debug", declarativeDebug);
#endif

#ifdef KCL_performancedatamanager
    engine->rootContext()->setContextProperty("PerformanceDataManager", &globalPerformanceDataManager);
#endif

#ifdef KCL_progressmanager
    engine->rootContext()->setContextProperty("ProgressManager", &globalProgressManager);
#endif

#ifdef KCL_base64imageprovider
    engine->addImageProvider("base64", new Base64ImageProvider());
#endif
}

#if defined(KCL_PLUGIN) && QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(kclplugin, KCLPlugin)

#endif
