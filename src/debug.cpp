/*
 * Copyright (C) 2006-2013 Andre Beckedorf <evilJazz _AT_ katatstrophos _DOT_ net>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "debug.h"
#include "compathack.h"

#include <QHash>

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

static int indentlevel = -1;

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
        qDebug("%*s>> %s", ++indentlevel * 3, "", (const char*)name.toUtf8());
}

void kaDebugExitMethod(const QString &name)
{
    if (diagnosticOutputEnabled_)
        qDebug("%*s<< %s", indentlevel-- * 3, "", (const char*)name.toUtf8());
}

void kaDebugEnterMethod(const char *fileName, int line, const char *functionSignature, const QString &text)
{
    if (diagnosticOutputEnabled_)
        qDebug("%*s>> %s", ++indentlevel * 3, "", (const char*)kaFormatFunctionSignature(fileName, line, functionSignature, text).toUtf8());
}

void kaDebugExitMethod(const char *fileName, int line, const char *functionSignature, const QString &text)
{
    if (diagnosticOutputEnabled_)
        qDebug("%*s<< %s", indentlevel-- * 3, "", (const char*)kaFormatFunctionSignature(fileName, line, functionSignature, text).toUtf8());
}

void kaDebug(const QString &msg)
{
    if (diagnosticOutputEnabled_)
        qDebug("%*s%s", (indentlevel == -1 ? 0 : indentlevel * 3 + 3), "", (const char*)msg.toUtf8());
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
        kaDebug(QString().sprintf("%5d -> %100s, times called: %6d,  total runtime: %8d ms,  average runtime: %8.2f ms",
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
