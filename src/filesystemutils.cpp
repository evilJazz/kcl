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

#include "KCL/filesystemutils.h"

#include <QDir>
#include <QUrl>
#include <QFileInfo>
#include <QFileDialog>
#include <QDesktopServices>
#include <QCryptographicHash>

FileSystemUtils::FileSystemUtils(QObject *parent) :
    QObject(parent)
{
}

bool FileSystemUtils::fileExists(const QString &filename)
{
    return QFileInfo(QUrl(filename).toLocalFile()).exists();
}

bool FileSystemUtils::dirExists(const QString &dirname)
{
    return QDir(QUrl(dirname).toLocalFile()).exists();
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

QString FileSystemUtils::canonicalizePath(const QString &path, bool asURL)
{
    QString newName = QFileInfo(QUrl(path).toLocalFile()).canonicalFilePath();
    return asURL ? QUrl::fromLocalFile(newName).toString() : newName;
}

QString FileSystemUtils::localPath(const QUrl &url)
{
    return url.toLocalFile();
}

QString FileSystemUtils::pathAppend(const QString &path1, const QString &path2)
{
    return path1.endsWith("/") ? path1 + path2 : path1 + "/" + path2;
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

QString FileSystemUtils::homeLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::HomeLocation) + "/";
}

QString FileSystemUtils::documentsLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::DocumentsLocation) + "/";
}

QString FileSystemUtils::desktopLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::DesktopLocation) + "/";
}

QString FileSystemUtils::musicLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::MusicLocation) + "/";
}

QString FileSystemUtils::moviesLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::MoviesLocation) + "/";
}

QString FileSystemUtils::tempLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::TempLocation) + "/";
}

QString FileSystemUtils::dataLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::DataLocation) + "/";
}

QString FileSystemUtils::cacheLocation()
{
    return QDesktopServices::storageLocation(QDesktopServices::CacheLocation) + "/";
}
