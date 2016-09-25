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

#include "KCL/nativedialogs.h"

#include <QFileDialog>

NativeDialogs::NativeDialogs(QObject *parent) :
    QObject(parent)
{
}

QVariantMap NativeDialogs::getOpenFileName(const QString &caption, const QString &startWith, const QString &filter, bool resolveSymlinks)
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(0, caption, startWith, filter, &selectedFilter, resolveSymlinks ? QFlags<QFileDialog::Option>(0) : QFileDialog::DontResolveSymlinks);

    QVariantMap map;
    map["fileName"] = fileName;
    map["selectedFilter"] = selectedFilter;
    return map;
}

QVariantMap NativeDialogs::getSaveFileName(const QString &caption, const QString &startWith, const QString &filter, bool resolveSymlinks)
{
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(0, caption, startWith, filter, &selectedFilter, resolveSymlinks ? QFlags<QFileDialog::Option>(0) : QFileDialog::DontResolveSymlinks);

    QVariantMap map;
    map["fileName"] = fileName;
    map["selectedFilter"] = selectedFilter;
    return map;
}

QString NativeDialogs::getExistingDirectory(const QString &caption, const QString &dir, bool dirOnly, bool resolveSymlinks)
{
    return QFileDialog::getExistingDirectory(0, caption, dir,
                    (resolveSymlinks ? QFlags<QFileDialog::Option>(0) : QFileDialog::DontResolveSymlinks) |
                    (dirOnly ? QFileDialog::ShowDirsOnly : QFlags<QFileDialog::Option>(0)));
}

int NativeDialogs::information(const QString &title, const QString &text, NativeDialogs::StandardButton buttons, NativeDialogs::StandardButton defaultButton)
{
    return (int)QMessageBox::information(0, title, text, (QMessageBox::StandardButtons)buttons, (QMessageBox::StandardButton)defaultButton);
}

int NativeDialogs::question(const QString &title, const QString &text, NativeDialogs::StandardButton buttons, NativeDialogs::StandardButton defaultButton)
{
    return (int)QMessageBox::question(0, title, text, (QMessageBox::StandardButtons)buttons, (QMessageBox::StandardButton)defaultButton);
}

int NativeDialogs::warning(const QString &title, const QString &text, NativeDialogs::StandardButton buttons, NativeDialogs::StandardButton defaultButton)
{
    return (int)QMessageBox::warning(0, title, text, (QMessageBox::StandardButtons)buttons, (QMessageBox::StandardButton)defaultButton);
}

int NativeDialogs::critical(const QString &title, const QString &text, NativeDialogs::StandardButton buttons, NativeDialogs::StandardButton defaultButton)
{
    return (int)QMessageBox::critical(0, title, text, (QMessageBox::StandardButtons)buttons, (QMessageBox::StandardButton)defaultButton);
}
