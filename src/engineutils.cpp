#include "../include/KCL/engineutils.h"

#include "KCL/debug.h"

#include <QPixmapCache>
#include <QNetworkAccessManager>
#include <QAbstractNetworkCache>

#ifdef KCL_QTQUICK2
    #include <QQmlEngine>
    #define QDeclarativeEngine QQmlEngine
#else
    #include <QDeclarativeEngine>
    #include <QtScript/QScriptEngine>
    #include <QtScript/QScriptContext>
    #include <QtScript/QScriptContextInfo>
    #include "qdeclarativedebughelper_p.h"
#endif

EngineUtils::EngineUtils(QQmlEngine *engine) :
    QObject(engine),
    engine_(engine)
{
#ifndef KCL_QTQUICK2
    scriptEngine_ = QDeclarativeDebugHelper::getScriptEngine(engine_);
#endif
}

EngineUtils::~EngineUtils()
{
}

void EngineUtils::collectGarbage()
{
#ifdef KCL_QTQUICK2
    engine_->collectGarbage();
#else
    if (scriptEngine_)
        scriptEngine_->collectGarbage();
#endif
}

void EngineUtils::clearComponentCache()
{
    engine_->clearComponentCache();
}

void EngineUtils::trimComponentCache()
{
#ifdef KCL_QTQUICK2
    engine_->trimComponentCache();
#endif
}

void EngineUtils::clearNetworkCache()
{
    engine_->networkAccessManager()->clearAccessCache();

    if (engine_->networkAccessManager()->cache())
        engine_->networkAccessManager()->cache()->clear();
}

void EngineUtils::clearPixmapCache()
{
    QPixmapCache::clear();
}
