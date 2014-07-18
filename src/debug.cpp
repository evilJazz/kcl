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

#include "KCL/debug.h"

#include <QHash>
#include <QThread>

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

static int indentlevel = -1;

static KaMessageHandlerFunc customMessageHandler = NULL;

void kaInstallMessageHandler(KaMessageHandlerFunc customFunc)
{
    customMessageHandler = customFunc;
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

QString kaFormatFunctionSignature(const char *fileName, int line, const char *functionSignature, const QString &text)
{
    QString result = QString().sprintf("[%s:%d] : %s", fileName, line, functionSignature);

    if (!text.isEmpty())
        result += " : " + text;

    return result;
}

void kaDebugEnterMethod(const QString &name)
{
    if (diagnosticOutputEnabled_)
    {
        kaDebug(">> " + name);
        ++indentlevel;
    }
}

void kaDebugExitMethod(const QString &name)
{
    if (diagnosticOutputEnabled_)
    {
        --indentlevel;
        kaDebug("<< " + name);
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
    if (diagnosticOutputEnabled_)
    {
        if (customMessageHandler)
            customMessageHandler(QString().sprintf("[%p]%*s%s", (void *)QThread::currentThreadId(), (indentlevel == -1 ? 0 : indentlevel * 3 + 3), "", (const char*)msg.toUtf8()));
        else
            qDebug("[%p]%*s%s", (void *)QThread::currentThreadId(), (indentlevel == -1 ? 0 : indentlevel * 3 + 3), "", (const char*)msg.toUtf8());
    }
}

void kaFatal(const QString &msg)
{
    qFatal("FATAL: %*s%s", (indentlevel == -1 ? 0 : indentlevel * 3 + 3), "", (const char*)msg.toUtf8());
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

    if (timed)
    {
        timer_ = new QTime();
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
        int elapsed = timer_->elapsed();

        if (profile_)
            kaProfileAddRecord(string_, elapsed);

        string_ += QString().sprintf(" - [Timing] %5d ms", elapsed);
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
