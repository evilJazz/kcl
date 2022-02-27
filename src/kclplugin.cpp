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

#ifdef KCL_bytearrayutils
    #include "KCL/bytearrayutils.h"
#endif

#ifdef KCL_cursorarea
    #include "KCL/cursorarea.h"
#endif

#ifdef KCL_touchdetector
    #include "KCL/touchdetector.h"
#endif

#ifdef KCL_rawmousearea
    #include "KCL/rawmousearea.h"
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

#ifdef KCL_filesystemwatcher
    #include "KCL/filesystemwatcher.h"
#endif

#ifdef KCL_binaryfiledownloader
    #include "KCL/binaryfiledownloader.h"
#endif

#ifdef KCL_cookiejarinterface
    #include "KCL/cookiejarinterface.h"
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

#ifdef KCL_propertychangeobserver
    #include "KCL/propertychangeobserver.h"
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

#ifdef KCL_filteredimageprovider
    #include "KCL/filteredimageprovider.h"
#endif

#ifdef KCL_sortfiltermodel
    #include "KCL/sortfiltermodel.h"
#endif

#ifdef KCL_simplebase
    #include "KCL/simplebase.h"
#endif

#ifdef KCL_engineutils
    #include "KCL/engineutils.h"
#endif

#ifdef KCL_systemutils
    #include "KCL/systemutils.h"
#endif

#ifdef KCL_templaterenderer
    #include "KCL/templaterenderer.h"
#endif

#define kclRegisterMetaType(T, uri, versionMajor, versionMinor, typeName) \
    qmlRegisterType<T>(uri, versionMajor, versionMinor, typeName); \
    qRegisterMetaType<T*>(typeName);


void KCLPlugin::registerTypes(const char *uri)
{
    //@uri KCL

#ifdef KCL_binaryfiledownloader
    kclRegisterMetaType(BinaryFileDownloader, uri, 1, 0, "BinaryFileDownloader");
#endif

#ifdef KCL_propertychangeobserver
    kclRegisterMetaType(PropertyChangeObserver, uri, 1, 0, "PropertyChangeObserver");
#endif

#ifdef KCL_templaterenderer
    kclRegisterMetaType(TemplateRenderer, uri, 1, 0, "NativeTemplateRenderer");
#endif

#ifdef KCL_settingsgroup
    kclRegisterMetaType(SettingsGroup, uri, 1, 0, "SettingsGroup");
#endif

#ifdef KCL_objectutils
    qRegisterMetaType<ObjectUtils *>();
#endif

#ifdef KCL_bytearrayutils
    qRegisterMetaType<ByteArrayUtils *>();
#endif

#ifdef KCL_colorutils
    qRegisterMetaType<ColorUtils *>();
#endif

#ifdef KCL_filesystemutils
    qmlRegisterUncreatableType<CryptographicHash>(uri, 1, 0, "CryptographicHash", "Use FsUtils::getHasher to create a CryptographicHash.");
    qmlRegisterUncreatableType<FileInfo>(uri, 1, 0, "FileInfo", "Use FsUtils::getFileInfo to create a FileInfo.");
    qmlRegisterUncreatableType<IODevice>(uri, 1, 0, "IODevice", "Use FsUtils::getFile to create a IODevice.");
    qmlRegisterUncreatableType<FileDevice>(uri, 1, 0, "FileDevice", "Use FsUtils::getFile to create a FileDevice.");

    qRegisterMetaType<CryptographicHash *>();
    qRegisterMetaType<FileInfo *>();
    qRegisterMetaType<FileInfo::FileTime>("FileInfo::FileTime");
    qRegisterMetaType<IODevice *>();
    qRegisterMetaType<FileDevice *>();

#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    qRegisterMetaType<QFile::FileTime>("QFile::FileTime");
#endif
    qRegisterMetaType<QFile::Permissions>("QFile::Permissions");

    qRegisterMetaType<QDir::Filters>("QDir::Filters");
    qRegisterMetaType<QDir::SortFlags>("QDir::SortFlags");

    qRegisterMetaType<QIODevice::OpenMode>("QIODevice::OpenMode");
    qRegisterMetaType<QFile::FileError>("QFile::FileError");

    qRegisterMetaType<CryptographicHash::Algorithm>("CryptographicHash::Algorithm");
#endif

#ifdef KCL_filescanner
    kclRegisterMetaType(FileScanner, uri, 1, 0, "FileScanner");
#endif

#ifdef KCL_filesystemwatcher
    kclRegisterMetaType(FileSystemWatcher, uri, 1, 0, "FileSystemWatcher");
#endif

#ifdef KCL_cursorarea
    kclRegisterMetaType(CursorArea, uri, 1, 0, "CursorArea");
#endif

#ifdef KCL_touchdetector
    kclRegisterMetaType(TouchDetector, uri, 1, 0, "TouchDetector");
#endif

#ifdef KCL_rawmousearea
    qmlRegisterType<DeclarativeMouseEvent>();
    kclRegisterMetaType(RawMouseArea, uri, 1, 0, "RawMouseArea");
#endif

#ifdef KCL_imagerendersurface
    kclRegisterMetaType(ImageRenderSurface, uri, 1, 0, "ImageRenderSurface");
#endif

    kclRegisterMetaType(QTimer, uri, 1, 0, "QtTimer");

#ifdef KCL_performancedatamanager
    qmlRegisterType<PerformanceData>();
    qmlRegisterType<PerformanceDataAspect>();
    qmlRegisterType<PerformanceDataDetail>();
#endif

#ifdef KCL_progressmanager
    qmlRegisterType<ProgressManager>();
    qmlRegisterType<ProgressContext>();
#endif

#ifdef KCL_keyeventfilter
    qmlRegisterType<DeclarativeKeyEvent>();
    qmlRegisterType<KeyEventFilter>();
#endif

#ifdef KCL_logging
    qmlRegisterType<Logging>();
#endif

#ifdef KCL_declarativedebug
    qmlRegisterType<DeclarativeDebug>();
#endif

#ifdef KCL_sortfiltermodel
    qmlRegisterType<QAbstractItemModel>();
    kclRegisterMetaType(SortFilterModel, uri, 1, 0, "SortFilterModel");
#endif

#ifdef KCL_simplebase
    qmlRegisterType<SimpleBase>();
#endif

#ifdef KCL_engineutils
    qmlRegisterType<EngineUtils>();
#endif
}

#define kclInitializeDeclarativeSingleton(T, var, engine, singletonName) \
    qRegisterMetaType<T *>(); \
    T *var = new T(engine); \
    engine->rootContext()->setContextProperty(singletonName, var);

void KCLPlugin::initializeEngine(QDeclarativeEngine *engine, const char *uri)
{
    Q_UNUSED(uri);

    engine->addImportPath(":/"); // to allow "import KCL 1.0"

#ifdef KCL_filesystemutils
    kclInitializeDeclarativeSingleton(FileSystemUtils, fsUtils, engine, "FsUtils");
#endif

#ifdef KCL_objectutils
    kclInitializeDeclarativeSingleton(ObjectUtils, objectUtils, engine, "ObjectUtils");
#endif

#ifdef KCL_bytearrayutils
    kclInitializeDeclarativeSingleton(ByteArrayUtils, byteArrayUtils, engine, "ByteArrayUtils");
#endif

#ifdef KCL_colorutils
    kclInitializeDeclarativeSingleton(ColorUtils, colorUtils, engine, "ColorUtils");
#endif

#ifdef KCL_graphicsutils
    kclInitializeDeclarativeSingleton(GraphicsUtils, graphicsUtils, engine, "GraphicsUtils");
#endif

#ifdef KCL_imageutils
    kclInitializeDeclarativeSingleton(ImageUtils, imageUtils, engine, "ImageUtils");
#endif

#ifdef KCL_sceneutils
    kclInitializeDeclarativeSingleton(SceneUtils, sceneUtils, engine, "SceneUtils");
#endif

#ifdef KCL_datetimeutils
    kclInitializeDeclarativeSingleton(DateTimeUtils, dateTimeUtils, engine, "DateTimeUtils");
#endif

#ifdef KCL_networkutils
    kclInitializeDeclarativeSingleton(NetworkUtils, networkUtils, engine, "NetworkUtils");
#endif

#ifdef KCL_cookiejarinterface
    kclInitializeDeclarativeSingleton(CookieJarInterface, cookieJar, engine, "CookieJar");
#endif

#ifdef KCL_nativedialogs
    kclInitializeDeclarativeSingleton(NativeDialogs, nativeDialogs, engine, "NativeDialogs");
#endif

#ifdef KCL_logging
    engine->rootContext()->setContextProperty("Logging", &globalLogging);
#endif

#ifdef KCL_declarativedebug
    kclInitializeDeclarativeSingleton(DeclarativeDebug, debug, engine, "Debug");
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

#ifdef KCL_filteredimageprovider
    engine->addImageProvider("filtered", new FilteredImageProvider());
#endif

#ifdef KCL_simplebase
    kclInitializeDeclarativeSingleton(SimpleBase, simpleBase, engine, "SimpleBase");
#endif

#ifdef KCL_engineutils
    kclInitializeDeclarativeSingleton(EngineUtils, engineUtils, engine, "EngineUtils");
#endif

#ifdef KCL_systemutils
    kclInitializeDeclarativeSingleton(SystemUtils, systemUtils, engine, "SystemUtils");
#endif
}

#if defined(KCL_PLUGIN) && QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
Q_EXPORT_PLUGIN2(kclplugin, KCLPlugin)

#endif
