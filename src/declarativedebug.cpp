#include "KCL/declarativedebug.h"

#include "KCL/debug.h"

#include <QDeclarativeEngine>

#include <QtScript/QScriptEngine>
#include <QtScript/QScriptContext>
#include <QtScript/QScriptContextInfo>

#include "qdeclarativedebughelper_p.h"

KCL_USE_NAMESPACE

const QString cIdentifier("QML/JS");

DeclarativeDebug::DeclarativeDebug(QDeclarativeEngine *engine) :
    QObject(engine),
    engine_(engine)
{
    scriptEngine_ = QDeclarativeDebugHelper::getScriptEngine(engine_);
}

DeclarativeDebug::~DeclarativeDebug()
{
}

QString DeclarativeDebug::getFunctionSignature(QScriptContextInfo *info)
{
    return info->functionName() + "(" + info->functionParameterNames().join(", ") + ")";
}

void DeclarativeDebug::enterMethod(const QString &text)
{
    if (diagnosticOutputEnabled() && scriptEngine_)
    {
        QScriptContext *context = scriptEngine_->currentContext()->parentContext();
        if (context)
        {
            QScriptContextInfo info(context);
            kaDebugEnterMethod((cIdentifier + ": " + info.fileName()).toUtf8().constData(), info.lineNumber(), getFunctionSignature(&info).toUtf8().constData(), text);
        }
    }
}

void DeclarativeDebug::exitMethod(const QString &text)
{
    if (diagnosticOutputEnabled() && scriptEngine_)
    {
        QScriptContext *context = scriptEngine_->currentContext()->parentContext();
        if (context)
        {
            QScriptContextInfo info(context);
            kaDebugExitMethod((cIdentifier + ": " + info.fileName()).toUtf8().constData(), info.lineNumber(), getFunctionSignature(&info).toUtf8().constData(), text);
        }
    }
}

void DeclarativeDebug::print(const QString &text)
{
    if (diagnosticOutputEnabled())
        kaDebug("[" + cIdentifier + "] " + text);
}
