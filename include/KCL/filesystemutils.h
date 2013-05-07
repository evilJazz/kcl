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

#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QUrl>
#include <QVariantMap>

class KCL_EXPORT FileSystemUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString homeLocation READ homeLocation CONSTANT)
    Q_PROPERTY(QString documentsLocation READ documentsLocation CONSTANT)
    Q_PROPERTY(QString desktopLocation READ desktopLocation CONSTANT)
    Q_PROPERTY(QString musicLocation READ musicLocation CONSTANT)
    Q_PROPERTY(QString moviesLocation READ moviesLocation CONSTANT)
    Q_PROPERTY(QString tempLocation READ tempLocation CONSTANT)
    Q_PROPERTY(QString dataLocation READ dataLocation CONSTANT)
    Q_PROPERTY(QString cacheLocation READ cacheLocation CONSTANT)
public:
    explicit FileSystemUtils(QObject *parent = 0);

    Q_INVOKABLE static bool fileExists(const QString &filename);
    Q_INVOKABLE static bool dirExists(const QString &dirname);
    Q_INVOKABLE static bool isDir(const QString &dirname);

    Q_INVOKABLE static QString extractFileName(const QString &filename);
    Q_INVOKABLE static QString extractPath(const QString &filename);

    Q_INVOKABLE static QString extractBaseName(const QString &filename);
    Q_INVOKABLE static QString extractCompleteBaseName(const QString &filename);

    Q_INVOKABLE static QString extractSuffix(const QString &filename);
    Q_INVOKABLE static QString extractCompleteSuffix(const QString &filename);

    Q_INVOKABLE static QString canonicalizePath(const QString &path, bool asURL);
    Q_INVOKABLE static QString localPath(const QUrl &url);

    Q_INVOKABLE static QString pathAppend(const QString &path1, const QString &path2);

    Q_INVOKABLE static QString formatFileSize(long long fileSize);

    Q_INVOKABLE static QString md5HashFile(const QString &fileName);
    Q_INVOKABLE static QByteArray getContents(const QString &fileName);
    Q_INVOKABLE static bool putContents(const QString &fileName, const QByteArray &contents, bool append = false);

    static QString homeLocation();
    static QString documentsLocation();
    static QString desktopLocation();
    static QString musicLocation();
    static QString moviesLocation();
    static QString tempLocation();
    static QString dataLocation();
    static QString cacheLocation();

signals:

public slots:

};

#endif // FILESYSTEMUTILS_H
