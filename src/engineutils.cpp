#include "../include/KCL/engineutils.h"

#include "KCL/debug.h"
#include "KCL/objectutils.h"

#include <QPixmapCache>
#include <QNetworkAccessManager>
#include <QAbstractNetworkCache>

#ifdef KCL_QTQUICK2
    #include <QQmlEngine>
    #include <QQmlContext>
    #include <QQmlComponent>
    #include <QQmlProperty>
    #include <QQmlExpression>
    #define QDeclarativeEngine QQmlEngine
    #define QDeclarativeComponent QQmlComponent
    #define QDeclarativeContext QQmlContext
#else
    #include <QDeclarativeEngine>
    #include <QDeclarativeContext>
    #include <QDeclarativeComponent>
    #include <QtScript/QScriptEngine>
    #include <QtScript/QScriptContext>
    #include <QtScript/QScriptContextInfo>
    #include "qdeclarativedebughelper_p.h"
#endif

EngineUtils::EngineUtils(QDeclarativeEngine *engine) :
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
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    engine_->networkAccessManager()->clearAccessCache();
#endif

    if (engine_->networkAccessManager()->cache())
        engine_->networkAccessManager()->cache()->clear();
}

void EngineUtils::clearPixmapCache()
{
    QPixmapCache::clear();
}

void EngineUtils::setContextProperty(QObject *target, const QString &propertyName, QVariant value)
{
    QDeclarativeContext *targetContext = engine_->contextForObject(target);
    if (targetContext)
        targetContext->setContextProperty(propertyName, value);
}

void EngineUtils::setContextProperty(QObject *target, const QString &propertyName, QObject *value)
{
    QDeclarativeContext *targetContext = engine_->contextForObject(target);
    if (targetContext)
        targetContext->setContextProperty(propertyName, value);
}

void EngineUtils::setContextObject(QObject *target, QObject *source)
{
    QDeclarativeContext *targetContext = engine_->contextForObject(target);
    if (targetContext)
        targetContext->setContextObject(source);
}

QObject *EngineUtils::createObjectWithContextObject(QDeclarativeComponent *component, QObject *contextObject)
{
    QDeclarativeContext *rootContext = QDeclarativeEngine::contextForObject(contextObject);
    if (!rootContext)
        rootContext = engine_->rootContext();

    QDeclarativeContext *itemContext = new QDeclarativeContext(rootContext);
    itemContext->setContextObject(contextObject);

    QObject *object = component->create(itemContext);

    if (object)
        QDeclarativeEngine::setObjectOwnership(object, QDeclarativeEngine::JavaScriptOwnership);

    return object;
}

#ifdef KCL_EXPERIMENTAL
#ifdef KCL_QTQUICK2

QVariantMap EngineUtils::getMetaObjectInfo(QJSValue value, QObject *contextObject)
{
    QVariantMap result;

    if (value.isString())
    {
        /*
        QQmlProperty prop(engine_->rootContext()->contextObject(), value.toString(), engine_->rootContext());

        result.insert("name", prop.name());
        result.insert("isValid", prop.isValid());
        result.insert("isProperty", prop.isProperty());
        result.insert("propertyType", prop.propertyType());
        result.insert("propertyTypeName", prop.propertyTypeName());
        */

        QQmlContext *context = engine_->rootContext();

        if (contextObject)
            context = QDeclarativeEngine::contextForObject(contextObject);
        else
            contextObject = engine_->rootContext()->contextObject();

        QQmlExpression *expr = new QQmlExpression(context, contextObject, value.toString());
        QVariant ev = expr->evaluate();

        result.insert("error", expr->error().description());
        result.insert("hasError", expr->hasError());

        if (!expr->hasError() && ev.isValid())
        {
            QObject *evObj = ObjectUtils::objectify(ev);
            if (evObj)
            {
                result.insert("resultTypeName", QString(evObj->metaObject()->className()));
            }
            else
            {
                result.insert("resultTypeName", ev.typeName());
            }
        }

        delete expr;
    }
    /*
    else if (value.isQMetaObject())
    {
        //result << value.toQMetaObject()->className();
    }
    */
    else
    {
        result.insert("error", "Unknown type.");
        result.insert("hasError", true);
    }

    return result;
}

QVariantMap EngineUtils::execute(const QString &codeSnippet, QObject *contextObject)
{
    QVariantMap result;

    QQmlContext *context = engine_->rootContext();

    if (contextObject)
        context = QDeclarativeEngine::contextForObject(contextObject);
    else
        contextObject = engine_->rootContext()->contextObject();

    QScopedPointer<QQmlExpression> expr(new QQmlExpression(context, contextObject, codeSnippet));
    QVariant ev = expr->evaluate();

    result.insert("result", ev);
    result.insert("error", expr->error().description());
    result.insert("hasError", expr->hasError());

    return result;
}

#endif
#endif
