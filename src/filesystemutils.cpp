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

#include "KCL/filesystemutils.h"

#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QCryptographicHash>

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
#include <QStandardPaths>
#else
#include <QDesktopServices>
#endif

#ifndef _MSC_VER
#include <unistd.h>
#endif

FileSystemUtils::FileSystemUtils(QObject *parent) :
    QObject(parent)
{
}

bool FileSystemUtils::fileExists(const QString &filename)
{
    return QFileInfo(filename).exists();
}

bool FileSystemUtils::dirExists(const QString &dirname)
{
    return QDir(dirname).exists();
}

bool FileSystemUtils::isDir(const QString &dirname)
{
    return QFileInfo(dirname).isDir();
}

QString FileSystemUtils::extractFileName(const QString &filename)
{
    return QFileInfo(filename).fileName();
}

QString FileSystemUtils::extractPath(const QString &filename)
{
    return QFileInfo(filename).path();
}

QString FileSystemUtils::extractBaseName(const QString &filename)
{
    return QFileInfo(filename).baseName();
}

QString FileSystemUtils::extractCompleteBaseName(const QString &filename)
{
    return QFileInfo(filename).completeBaseName();
}

QString FileSystemUtils::extractSuffix(const QString &filename)
{
    return QFileInfo(filename).suffix();
}

QString FileSystemUtils::extractCompleteSuffix(const QString &filename)
{
    return QFileInfo(filename).completeSuffix();
}

QVariant FileSystemUtils::absoluteFilePath(const QString &filePath)
{
    return QFileInfo(filePath).absoluteFilePath();
}

QVariant FileSystemUtils::canonicalFilePath(const QString &filePath)
{
    return QFileInfo(filePath).canonicalFilePath();
}

QUrl FileSystemUtils::urlFromString(const QString &url)
{
    return QUrl(url);
}

QString FileSystemUtils::stringFromUrl(const QUrl &url)
{
    return url.toString();
}

QUrl FileSystemUtils::urlFromLocalPath(const QString &path)
{
    if (path.startsWith(":/"))
        return "qrc" + path;
    else
        return QUrl::fromLocalFile(path);
}

QString FileSystemUtils::localPathFromUrl(const QUrl &url)
{
    if (url.toString().startsWith("qrc:"))
        return url.toString().replace("qrc:", ":");
    else
        return url.scheme() == "file" ? url.toLocalFile() : "";
}

QString FileSystemUtils::pathAppend(const QString &path1, const QString &path2)
{
    if (path1.isEmpty())
        return path2;
    else if (path2.isEmpty())
        return path1;
    else if (path1.endsWith('/'))
        return path1 + path2;
    else
        return path1 + "/" + path2;
}

QDateTime FileSystemUtils::lastModified(const QString &path)
{
    return QFileInfo(path).lastModified();
}

QDateTime FileSystemUtils::created(const QString &path)
{
    return QFileInfo(path).created();
}

QString FileSystemUtils::findNextParent(const QString &path)
{
    QString parent = path;
    QChar sep('/');

    if (parent.endsWith(sep))
        parent = parent.left(parent.size() - 1);

    int index;

    do
    {
        index = parent.lastIndexOf(sep);

        if (index == -1)
            return QString();

        parent = parent.left(index);
    }
    while (!QDir(parent).exists());

    return parent;
}

QString FileSystemUtils::formatFileSize(long long fileSize)
{
    QString result;

    if (fileSize > 1024 * 1024 * 1024)
        result = QString().sprintf("%.2f GB", (float)fileSize / (1024 * 1024 * 1024));
    else if (fileSize > 1024 * 1024)
        result = QString().sprintf("%.2f MB", (float)fileSize / (1024 * 1024));
    else
        result = QString().sprintf("%.1f KB", (float)fileSize / 1024);

    return result;
}

QString FileSystemUtils::md5HashFile(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
    {
        QCryptographicHash crypto(QCryptographicHash::Md5);

        while (!file.atEnd())
            crypto.addData(file.read(8192));

        return crypto.result().toHex();
    }

    return QString::null;
}

bool FileSystemUtils::putContents(const QString &fileName, const QByteArray &contents, bool append)
{
    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly))
    {
        if (append)
            file.seek(file.size());

        return file.write(contents) == contents.size();
    }

    return false;
}

QByteArray FileSystemUtils::getContents(const QString &fileName)
{
    QFile file(fileName);
    if (file.open(QIODevice::ReadOnly))
        return file.readAll();

    return QByteArray();
}

bool FileSystemUtils::forceDirectory(const QString &dirName)
{
    if (QFileInfo(dirName).exists())
        return true;

    QDir dir;
    return dir.mkpath(dirName);
}

bool FileSystemUtils::removeDirectoryRecursively(const QString &dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName))
    {
        foreach (QFileInfo info, dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst))
        {
            if (info.isDir())
                result = removeDirectoryRecursively(info.absoluteFilePath());
            else
                result = QFile::remove(info.absoluteFilePath());

            if (!result)
                return result;
        }

        result = dir.rmdir(dirName);
    }

    return result;
}

bool FileSystemUtils::removeFile(const QString &fileName)
{
    return QFile::remove(fileName);
}

bool FileSystemUtils::renameFile(const QString &oldFileName, const QString &newFileName)
{
    return QFile::rename(oldFileName, newFileName);
}

bool FileSystemUtils::copyFile(const QString &srcFileName, const QString &destFileName)
{
    return QFile::copy(srcFileName, destFileName);
}

void FileSystemUtils::syncToDisk()
{
#ifdef Q_OS_LINUX
    sync();
#else
    qWarning("FileSystemUtils::syncToDisk() is not implemented for this OS.");
#endif
}

QString FileSystemUtils::homeLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/";
#endif
}

QString FileSystemUtils::documentsLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + "/";
#endif
}

QString FileSystemUtils::desktopLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::DesktopLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/";
#endif
}

QString FileSystemUtils::picturesLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::PicturesLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::PicturesLocation) + "/";
#endif
}

QString FileSystemUtils::musicLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::MusicLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::MusicLocation) + "/";
#endif
}

QString FileSystemUtils::moviesLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::MoviesLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::MoviesLocation) + "/";
#endif
}

QString FileSystemUtils::tempLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::TempLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "/";
#endif
}

QString FileSystemUtils::dataLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::DataLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/";
#endif
}

QString FileSystemUtils::cacheLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::CacheLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::CacheLocation) + "/";
#endif
}

QString FileSystemUtils::applicationsLocation()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
    return QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation) + "/";
#else
    return QDesktopServices::storageLocation(QDesktopServices::ApplicationsLocation) + "/";
#endif
}
