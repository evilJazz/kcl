/*
 * Copyright (C) 2006-2013 Andre Beckedorf <evilJazz _AT_ katastrophos _DOT_ net>
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

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include <time.h>

#include <qobject.h>
#include <qstring.h>
#ifdef QT4
#include <QTime>
#else
#include <qdatetime.h>
#endif

#ifdef Q_OS_SYMBIAN
#define D_FUNC_INFO __PRETTY_FUNCTION__
#else
#define D_FUNC_INFO Q_FUNC_INFO
#endif

void setDiagnosticOutputEnabled(bool value);
bool diagnosticOutputEnabled();

QString kaFormatFunctionSignature(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null);
void kaDebugEnterMethod(const QString &name);
void kaDebugExitMethod(const QString &name);
void kaDebugEnterMethod(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null);
void kaDebugExitMethod(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null);
void kaDebug(const QString &msg);
void kaFatal(const QString &msg);
void kaPrintMemStat();

void kaProfileAddRecord(const QString &name, int elapsed);
void kaProfilePrintStat();
void kaProfileClearStat();

class KaDebugGuard
{
public:
    KaDebugGuard(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null, bool timed = false, bool silence = false, bool profile = false);
    ~KaDebugGuard();

private:
    QString string_;
    bool silence_;
    bool profile_;
    QTime *timer_;
};

#if defined(DEBUG) || defined(DIAGNOSTIC_OUTPUT)
  #define DOP(...) __VA_ARGS__
  #define DHOP(...)
#else
  #define DOP(...)
  #define DHOP(...)
#endif

#define DGUARDMETHOD DOP(volatile KaDebugGuard guard(__FILE__, __LINE__, D_FUNC_INFO))
#define DGUARDMETHODTIMED DOP(volatile KaDebugGuard guard(__FILE__, __LINE__, D_FUNC_INFO, QString::null, true))
#define DGUARDMETHODPROFILED DOP(volatile KaDebugGuard guard(__FILE__, __LINE__, D_FUNC_INFO, QString::null, true, true, true))

#define DPROFILEPRINTSTAT DOP(kaProfilePrintStat())
#define DPROFILECLEARSTAT DOP(kaProfileClearStat())

#define DGUARDMETHODF(...) DOP(volatile KaDebugGuard guard(__FILE__, __LINE__, D_FUNC_INFO, QString().sprintf(__VA_ARGS__)))
#define DGUARDMETHODFTIMED(...) DOP(volatile KaDebugGuard guard(__FILE__, __LINE__, D_FUNC_INFO, QString().sprintf(__VA_ARGS__), true))

#define DHGUARDMETHOD DHOP(DGUARDMETHOD)
#define DHGUARDMETHODF(...) DHOP(DGUARDMETHODF(__VA_ARGS__))

#define DHGUARDMETHODTIMED DHOP(DGUARDMETHODTIMED)
#define DHGUARDMETHODFTIMED(...) DHOP(DGUARDMETHODFTIMED(__VA_ARGS__))

#define DENTERMETHOD DOP(kaDebugEnterMethod(__FILE__, __LINE__, D_FUNC_INFO))
#define DEXITMETHOD DOP(kaDebugExitMethod(__FILE__, __LINE__, D_FUNC_INFO))

#define DENTERMETHODF(...) DOP(kaDebugEnterMethod(__FILE__, __LINE__, D_FUNC_INFO, QString().sprintf(__VA_ARGS__)))
#define DEXITMETHODF(...) DOP(kaDebugExitMethod(__FILE__, __LINE__, D_FUNC_INFO, QString().sprintf(__VA_ARGS__)))

#define DHENTERMETHOD DHOP(DENTERMETHOD)
#define DHEXITMETHOD DHOP(DEXITMETHOD)

#define DHENTERMETHODF(...) DHOP(DENTERMETHODF(__VA_ARGS__))
#define DHEXITMETHODF(...) DHOP(DEXITMETHODF(__VA_ARGS__))

#define DPRINTF(...) DOP(kaDebug(QString().sprintf(__VA_ARGS__)))
#define DHPRINTF(...) DHOP(DPRINTF(__VA_ARGS__))

#define DTIMERINIT(timername) DOP(QTime timername; timername.start())
#define DTIMERSTART(timername) DOP(timername.start())
#define DTIMERPRINT(timername, description) DOP(kaDebug(QString().sprintf("[Timing] %-30s: %5d ms", description, timername.elapsed())))

#define DMEMSTAT() DOP(kaPrintMemStat())

#define DASSERT(cond, text) ((!(cond)) ? kaFatal(QString().sprintf("Assertion failed: %s, %s [in %s %s:%d]", #cond, text, D_FUNC_INFO, __FILE__, __LINE__)) : qt_noop())

#define WTF(...) kaFatal(QString().sprintf(__VA_ARGS__))

#endif /* DEBUG_H_INCLUDED */
