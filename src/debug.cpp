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

#include "KCL/debug.h"

#include <QHash>
#include <QThread>
#include <QDateTime>
#include <QMutex>
#include <QCoreApplication>

#include <QCoreApplication>

#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
#include <QLoggingCategory>
#endif

#include <stdio.h>
#include <sys/types.h>

#ifndef Q_OS_WIN
#include <unistd.h>
#endif

#ifdef Q_OS_LINUX
#include <stdlib.h>
#ifndef Q_OS_ANDROID
#include <execinfo.h>
#endif
#endif

static int indentLevel_ = -1;
static QElapsedTimer lastMsg_;
static QMutex *lastMsgMutex_ = NULL;

QMutex *getMutex()
{
    if (!lastMsgMutex_)
    {
        lastMsg_.restart();
        lastMsgMutex_ = new QMutex(QMutex::Recursive);
    }

    return lastMsgMutex_;
}

static KaMessageHandlerFunc customMessageHandler = NULL;

void kaInstallMessageHandler(KaMessageHandlerFunc customFunc)
{
    customMessageHandler = customFunc;
}

void forceEnableQtDebugOutput()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 2, 0)
    QLoggingCategory *defaultCategory = QLoggingCategory::defaultCategory();
    if (defaultCategory)
    {
        defaultCategory->setEnabled(QtDebugMsg, true);
        defaultCategory->setEnabled(QtWarningMsg, true);
#if QT_VERSION >= QT_VERSION_CHECK(5, 5, 0)
        defaultCategory->setEnabled(QtInfoMsg, true);
#endif
    }
#endif
}

#ifdef DEBUG
static bool diagnosticOutputEnabled_ = true;
#else
static bool diagnosticOutputEnabled_ = false;
#endif

void setDiagnosticOutputEnabled(bool value)
{
    diagnosticOutputEnabled_ = value;
}

bool diagnosticOutputEnabled()
{
    return diagnosticOutputEnabled_;
}

static qint64 showMarkerAfterMSecs_ = 5000;

void setShowMarkerAfter(int msecs)
{
    showMarkerAfterMSecs_ = msecs;
    lastMsg_.restart();
}

int showMarkerAfter()
{
    return showMarkerAfterMSecs_;
}

static bool showTimestamps_ = false;

void setTimestampsEnabled(bool value)
{
    showTimestamps_ = value;
}

bool timestampsEnabled()
{
    return showTimestamps_;
}

QString kaFormatFunctionSignature(const char *fileName, int line, const char *functionSignature, const QString &text)
{
    QByteArray fileNameBA(fileName);
    int lastSlash = fileNameBA.lastIndexOf('/');
    if (lastSlash > -1)
        fileNameBA = fileNameBA.mid(lastSlash + 1);

    QString result = QString().sprintf("[%s:%d] : %s", fileNameBA.constData(), line, functionSignature);

    if (!text.isEmpty())
        result += " : " + text;

    return result;
}

void kaDebugEnterMethod(const QString &name)
{
    if (diagnosticOutputEnabled_)
    {
        kaDebug("-> " + name);
        ++indentLevel_;
    }
}

void kaDebugExitMethod(const QString &name)
{
    if (diagnosticOutputEnabled_)
    {
        --indentLevel_;
        kaDebug("<- " + name);
    }
}

void kaDebugEnterMethod(const char *fileName, int line, const char *functionSignature, const QString &text)
{
    if (diagnosticOutputEnabled_)
        kaDebugEnterMethod(kaFormatFunctionSignature(fileName, line, functionSignature, text));
}

void kaDebugExitMethod(const char *fileName, int line, const char *functionSignature, const QString &text)
{
    if (diagnosticOutputEnabled_)
        kaDebugExitMethod(kaFormatFunctionSignature(fileName, line, functionSignature, text));
}

void kaDebug(const QString &msg)
{
    if (!qApp)
    {
        qWarning("ALARM! Qt application is not running!!!");
    }

    if (diagnosticOutputEnabled_)
    {
        QMutexLocker l(getMutex());

        if (showMarkerAfterMSecs_ > 0)
        {
            qint64 diff = lastMsg_.elapsed();
            if (lastMsg_.isValid() && diff > showMarkerAfterMSecs_)
            {
                lastMsg_.restart();
                kaDebug(QString("============================= last message was %1 ms ago. =============================").arg(diff));
            }
            else
                lastMsg_.restart();
        }

        int indent = indentLevel_ == -1 ? 0 : indentLevel_ * 3 + 3;
        QByteArray marker = (showTimestamps_ ? QByteArray::number(QDateTime::currentMSecsSinceEpoch()) + " " : "") +
                            (qApp ?
                                QString().sprintf("%16s %0x",
                                    (qApp->thread() == QThread::currentThread() ?
                                        "Main Thread" :
                                        QThread::currentThread()->objectName()
                                    ).toUtf8().constData(),
                                    QThread::currentThreadId()
                                ).toUtf8() :
                                "!"
                            );

        QString message = QString().sprintf("%s: %*s%s", marker.constData(), indent, "", (const char*)msg.toUtf8());

        if (customMessageHandler)
            customMessageHandler(message);
        else
            qDebug(message.toUtf8().constData());
    }
}

void kaFatal(const QString &msg)
{
    qFatal("FATAL: %*s%s", (indentLevel_ == -1 ? 0 : indentLevel_ * 3 + 3), "", (const char*)msg.toUtf8());
}

void kaPrintMemStat()
{
#ifdef Q_OS_LINUX
    char           buf[256];
    FILE         * file;
    unsigned int   pages;
    unsigned int   mem_size;

    snprintf(buf, sizeof(buf), "/proc/%d/statm", (unsigned int)getpid());

    if ((file = fopen( buf, "r" )) == NULL)
    {
        perror("open");
        return;
    }

    fgets(buf, sizeof(buf), file);

    fclose(file);

    sscanf(buf, "%u", &pages);
    mem_size = ((unsigned long)pages) * ((unsigned long)getpagesize());

    kaDebug(QString().sprintf("Memory used: %d bytes / %d kbytes", mem_size, mem_size / 1024));
#endif
}

void kaPrintBacktrace()
{
#if defined(Q_OS_LINUX) && !defined(Q_OS_ANDROID)
    void *array[10];
    int size = backtrace(array, 10);

    char **messages = backtrace_symbols(array, size);

    for (int i = 1; i < size && messages != NULL; ++i)
        kaDebug(QString().sprintf("BT: (%d) %s", i, messages[i]));

    free(messages);
#endif
}

/* KaDebugGuard */

KaDebugGuard::KaDebugGuard(const char *fileName, int line, const char *functionSignature, const QString &text, bool timed, bool silence, bool profile) :
    silence_(silence),
    profile_(profile),
    timer_(NULL)
{
    if (!diagnosticOutputEnabled_)
        silence_ = true;

    if (qApp && timed)
    {
        timer_ = new QElapsedTimer();
        timer_->start();
    }

    if (!silence_ || timer_)
        string_ = kaFormatFunctionSignature(fileName, line, functionSignature, text);

    if (!silence_)
        kaDebugEnterMethod(string_);
}

KaDebugGuard::~KaDebugGuard()
{
    if (timer_)
    {
        qint64 elapsed = timer_->elapsed();

        if (profile_)
            kaProfileAddRecord(string_, elapsed);

        string_ += QString().sprintf(" - [Timing] %5lld ms", elapsed);
        delete timer_;
        timer_ = NULL;
    }

    if (!silence_)
        kaDebugExitMethod(string_);
}

/* ProfileRecord */

struct ProfileRecord
{
    QString name;
    int timesCalled;
    qint64 totalRuntime;

    bool operator<(const ProfileRecord &other) const
    {
        return timesCalled < other.timesCalled;
    }
};

typedef QHash<QString, ProfileRecord> ProfileRecords;
static ProfileRecords *profileRecords = NULL;

ProfileRecords &profileRecordsInstance()
{
    if (!profileRecords)
        profileRecords = new ProfileRecords();

    return *profileRecords;
}

void kaProfileAddRecord(const QString &name, int elapsed)
{
    ProfileRecord &record = profileRecordsInstance()[name];
    if (record.name.isEmpty())
        record.name = name;

    ++record.timesCalled;
    record.totalRuntime += elapsed;
}

void kaProfilePrintStat()
{
    if (!diagnosticOutputEnabled_)
        return;

    QList<ProfileRecord> records = profileRecordsInstance().values();
    qSort(records);

    kaDebug("");
    kaDebug("Profile stats:");
    kaDebug("=============================");
    for (int i = 0; i < records.count(); ++i)
    {
        const ProfileRecord &record = records.at(i);
        kaDebug(QString().sprintf("%5d -> %100s, times called: %6d,  total runtime: %8lld ms,  average runtime: %8.2f ms",
            i + 1,
            record.name.toLatin1().constData(),
            record.timesCalled,
            record.totalRuntime,
            (float)record.totalRuntime / record.timesCalled
        ));
    }
    kaDebug("=============================");
    kaDebug("");
}

void kaProfileClearStat()
{
    profileRecordsInstance().clear();
}
