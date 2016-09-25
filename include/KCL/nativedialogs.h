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

#ifndef NATIVEDIALOGS_H
#define NATIVEDIALOGS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>
#include <QMessageBox>

class KCL_EXPORT NativeDialogs : public QObject
{
    Q_OBJECT
    Q_ENUMS(StandardButton)
    //Q_ENUMS(StandardButtons)
public:
    enum StandardButton {
        NoButton           = 0x00000000,
        Ok                 = 0x00000400,
        Save               = 0x00000800,
        SaveAll            = 0x00001000,
        Open               = 0x00002000,
        Yes                = 0x00004000,
        YesToAll           = 0x00008000,
        No                 = 0x00010000,
        NoToAll            = 0x00020000,
        Abort              = 0x00040000,
        Retry              = 0x00080000,
        Ignore             = 0x00100000,
        Close              = 0x00200000,
        Cancel             = 0x00400000,
        Discard            = 0x00800000,
        Help               = 0x01000000,
        Apply              = 0x02000000,
        Reset              = 0x04000000,
        RestoreDefaults    = 0x08000000
    };

    //typedef StandardButton StandardButtons;

    explicit NativeDialogs(QObject *parent = 0);
    
    Q_INVOKABLE QVariantMap getOpenFileName(
        const QString &caption = QString(),
        const QString &startWith = QString(),
        const QString &filter = QString(),
        bool resolveSymlinks = true
    );

    Q_INVOKABLE QVariantMap getSaveFileName(
        const QString &caption = QString(),
        const QString &startWith = QString(),
        const QString &filter = QString(),
        bool resolveSymlinks = true
    );

    Q_INVOKABLE QString getExistingDirectory(
        const QString & caption,
        const QString & dir,
        bool dirOnly,
        bool resolveSymlinks
    );
    
    // Can't use NativeDialogs::StandardButton as return value due to bug in Qt
    // Details: http://lists.qt-project.org/pipermail/interest/2011-December/000255.html
    Q_INVOKABLE int information(const QString &title,
         const QString &text, NativeDialogs::StandardButton buttons = Ok,
         NativeDialogs::StandardButton defaultButton = NoButton);

    Q_INVOKABLE int question(const QString &title,
         const QString &text, NativeDialogs::StandardButton buttons = Ok,
         NativeDialogs::StandardButton defaultButton = NoButton);

    Q_INVOKABLE int warning(const QString &title,
         const QString &text, NativeDialogs::StandardButton buttons = Ok,
         NativeDialogs::StandardButton defaultButton = NoButton);

    Q_INVOKABLE int critical(const QString &title,
         const QString &text, NativeDialogs::StandardButton buttons = Ok,
         NativeDialogs::StandardButton defaultButton = NoButton);

public slots:
    
};

#endif // NATIVEDIALOGS_H
