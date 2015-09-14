#include "KCL/declarativedebug.h"

#include "KCL/debug.h"

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

const QString cIdentifier("QML/JS");

void DeclarativeDebug::setOutputEnabled(bool enabled)
{
    if (enabled != outputEnabled_)
    {
        outputEnabled_ = enabled;
        emit outputEnabledChanged();
    }
}

DeclarativeDebug::DeclarativeDebug(QDeclarativeEngine *engine) :
    QObject(engine),
    outputEnabled_(true),
    engine_(engine)
{
#ifndef KCL_QTQUICK2
    scriptEngine_ = QDeclarativeDebugHelper::getScriptEngine(engine_);
#endif
}

DeclarativeDebug::~DeclarativeDebug()
{
}

#ifdef KCL_QTQUICK2
// Dummies for now, till we can get access to the current stack frame via QV4::ExecutionEngine and QV4::CallContext

void DeclarativeDebug::enterMethod(const QString &text)
{
    if (outputEnabled_ && diagnosticOutputEnabled())
    {
        kaDebugEnterMethod(QString(cIdentifier + ": <unknown>").toUtf8().constData(), 0, QString("<unknown method>").toUtf8().constData(), text);
    }
}

void DeclarativeDebug::exitMethod(const QString &text)
{
    if (outputEnabled_ && diagnosticOutputEnabled())
    {
        kaDebugExitMethod(QString(cIdentifier + ": <unknown>").toUtf8().constData(), 0, QString("<unknown method>").toUtf8().constData(), text);
    }
}
#else
QString DeclarativeDebug::getFunctionSignature(QScriptContextInfo *info)
{
    return info->functionName() + "(" + info->functionParameterNames().join(", ") + ")";
}

void DeclarativeDebug::enterMethod(const QString &text)
{
    if (outputEnabled_ && diagnosticOutputEnabled() && scriptEngine_)
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
    if (outputEnabled_ && diagnosticOutputEnabled() && scriptEngine_)
    {
        QScriptContext *context = scriptEngine_->currentContext()->parentContext();
        if (context)
        {
            QScriptContextInfo info(context);
            kaDebugExitMethod((cIdentifier + ": " + info.fileName()).toUtf8().constData(), info.lineNumber(), getFunctionSignature(&info).toUtf8().constData(), text);
        }
    }
}
#endif

void DeclarativeDebug::print(const QString &text)
{
    if (outputEnabled_ && diagnosticOutputEnabled())
        kaDebug("[" + cIdentifier + "] " + text);
}
