#include "kclplugin.h"

#include <QtDeclarative>

#include "settingsgroup.h"
#include "filesystemutils.h"
#include "filescanner.h"
#include "binaryfiledownloader.h"
#include "performancedatamanager.h"
#include "progressmanager.h"
#include "keyeventfilter.h"
#include "settingsgroup.h"
#include "logging.h"

void KCLPlugin::registerTypes(const char *uri)
{
    //@uri KCL
    qmlRegisterType<BinaryFileDownloader>(uri, 1, 0, "BinaryFileDownloader");
    qmlRegisterType<SettingsGroup>(uri, 1, 0, "SettingsGroup");
    qmlRegisterType<FileScanner>(uri, 1, 0, "FileScanner");

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
    FileSystemUtils *fsUtils = new FileSystemUtils(engine);
    engine->rootContext()->setContextProperty("fsUtils", fsUtils);

    engine->rootContext()->setContextProperty("performanceDataManager", &qPerformanceDataManager);
    engine->rootContext()->setContextProperty("progressManager", &globalProgressManager);
}

Q_EXPORT_PLUGIN2(styleplugin, KCLPlugin)
