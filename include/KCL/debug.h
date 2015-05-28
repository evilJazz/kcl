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

#ifndef DEBUG_H_INCLUDED
#define DEBUG_H_INCLUDED

#include "KCL/kcl_global.h"

#include <time.h>

#include <QObject>
#include <QString>
#include <QElapsedTimer>

#include <QtGlobal>

#ifdef Q_OS_SYMBIAN
#define D_FUNC_INFO __PRETTY_FUNCTION__
#else
#define D_FUNC_INFO Q_FUNC_INFO
#endif

KCL_EXPORT void setDiagnosticOutputEnabled(bool value);
KCL_EXPORT bool diagnosticOutputEnabled();

KCL_EXPORT void setShowMarkerAfter(int seconds);
KCL_EXPORT int showMarkerAfter();

typedef void (*KaMessageHandlerFunc)(const QString &msg);
KCL_EXPORT void kaInstallMessageHandler(KaMessageHandlerFunc customFunc);

KCL_EXPORT QString kaFormatFunctionSignature(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null);
KCL_EXPORT void kaDebugEnterMethod(const QString &name);
KCL_EXPORT void kaDebugExitMethod(const QString &name);
KCL_EXPORT void kaDebugEnterMethod(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null);
KCL_EXPORT void kaDebugExitMethod(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null);
KCL_EXPORT void kaDebug(const QString &msg);
KCL_EXPORT void kaFatal(const QString &msg);
KCL_EXPORT void kaPrintMemStat();
KCL_EXPORT void kaPrintBacktrace(); // requires -rdynamic gcc param for maximal effect...

KCL_EXPORT void kaProfileAddRecord(const QString &name, int elapsed);
KCL_EXPORT void kaProfilePrintStat();
KCL_EXPORT void kaProfileClearStat();

class KCL_EXPORT KaDebugGuard
{
public:
    KaDebugGuard(const char *fileName, int line, const char *functionSignature, const QString &text = QString::null, bool timed = false, bool silence = false, bool profile = false);
    ~KaDebugGuard();

private:
    QString string_;
    bool silence_;
    bool profile_;
    QElapsedTimer *timer_;
};

#if defined(DEBUG) || defined(DIAGNOSTIC_OUTPUT)
  #define DOP(...) __VA_ARGS__
  #ifdef DEBUG_NOISY
    #define DHOP(...) __VA_ARGS__
  #else
    #define DHOP(...)
  #endif
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

#define DTIMERINIT(timername) DOP(QElapsedTimer timername; timername.start())
#define DTIMERSTART(timername) DOP(timername.start())
#define DTIMERPRINT(timername, description) DOP(kaDebug(QString().sprintf("[Timing] %-30s: %5d ms", description, timername.elapsed())))

#define DMEMSTAT DOP(kaPrintMemStat())
#define DBACKTRACE DOP(kaPrintBacktrace())

#define DASSERT(cond, text) DOP((!(cond)) ? kaDebug(QString().sprintf("Assertion failed: %s, %s [in %s %s:%d]", #cond, text, D_FUNC_INFO, __FILE__, __LINE__)) : qt_noop())

#define WTF(...) kaFatal(QString().sprintf(__VA_ARGS__))

#endif /* DEBUG_H_INCLUDED */
