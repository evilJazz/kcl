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
    #include <QJSValueIterator>

    #ifdef KCL_QTQUICK_PRIVATE
    #include <private/qqmlcontext_p.h>
    #include <private/qv4identifier_p.h>
    #endif
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

QJSValue EngineUtils::globalObject()
{
    return engine_->globalObject();
}

QVariantList EngineUtils::getContextPropertiesAndGlobals(QObject *contextObject)
{
    QVariantList result;
    QVariantMap entry;

    QQmlContext *context = engine_->rootContext();

    if (contextObject)
        context = QDeclarativeEngine::contextForObject(contextObject);
    else
        contextObject = engine_->rootContext()->contextObject();

    if (context == engine_->rootContext())
    {
        // Get JS global objects / type prototypes
        QJSValueIterator it(engine_->globalObject());

        while (it.hasNext())
        {
            it.next();

            entry.insert("name", it.name());
            entry.insert("typeName", "JSGlobal");
            result << entry;
        }
    }

#ifdef KCL_QTQUICK_PRIVATE
    if (context)
    {
        QQmlContextData *contextData = QQmlContextData::get(context);

        if (contextData)
        {
            const QV4::IdentifierHash &propNames = contextData->propertyNames();

            QV4::IdentifierHashEntry *e = propNames.d->entries;
            QV4::IdentifierHashEntry *end = e + propNames.d->alloc;

            while (e < end)
            {
                QString propName;

#if QT_VERSION < QT_VERSION_CHECK(5, 12, 0)
                if (e->identifier)
                    propName = e->identifier->string;
#else
                if (e->identifier.isValid())
                    propName = e->identifier.toQString();
#endif

                if (!propName.isNull())
                {
                    entry.insert("name", propName);
                    entry.insert("typeName", "QMLContextProperty");
                    result << entry;
                }

                ++e;
            }
        }
#endif
    }

    return result;
}

QVariantMap EngineUtils::getMetaObjectInfo(QJSValue value, QObject *contextObject)
{
    QVariantMap result;

    if (value.isString())
    {
        result = EngineUtils::evaluateInContext(value.toString(), contextObject);
        QVariant ev = result.value("result");

        if (!result.value("hasError").toBool() && ev.isValid())
        {
            QObject *evObj = ObjectUtils::objectify(ev);

            if (evObj)
            {
                result = ObjectUtils::introspectObject(evObj);
                result.insert("resultTypeName", QString(evObj->metaObject()->className()));
            }
            else
                result.insert("resultTypeName", ev.typeName());
        }
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

QJSValue EngineUtils::evaluate(const QString &codeSnippet)
{
    return engine_->evaluate(codeSnippet);
}

QVariantMap EngineUtils::evaluateInContext(const QString &codeSnippet, QObject *contextObject)
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
