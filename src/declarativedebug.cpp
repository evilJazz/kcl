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
