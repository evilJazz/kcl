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

#ifndef LOGGING_H
#define LOGGING_H

#include "KCL/kcl_global.h"

#include <QObject>

namespace KCL {

class KCL_EXPORT Logging
{
public:
    static Logging& singleton();
#	define gLogging Logging::singleton()

    void registerHandler();
    void unregisterHandler();

#ifdef KCL_WIDGETS
    void enableLogWindow();
    void disableLogWindow();
#endif

    void enableLogFile(const QString &fileName);
    void disableLogFile();

signals:

public slots:

protected:
    explicit Logging();

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    static void customMessageHandler(QtMsgType type, const QMessageLogContext &, const QString &);
#else
    static void customMessageHandler(QtMsgType type, const char *msg);
#endif
};

}

#endif // LOGGING_H
