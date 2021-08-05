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

#ifndef FILESYSTEMUTILS_H
#define FILESYSTEMUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QUrl>
#include <QVariantMap>
#include <QDateTime>
#include <QDir>
#include <QPointer>
#include <QFile>
#include <QFileInfo>

#include <QCryptographicHash>

#include "KCL/filescanner.h"

class KCL_EXPORT IODevice : public QObject
{
    Q_OBJECT
    Q_ENUMS(OpenMode)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)

    Q_PROPERTY(bool isTextModeEnabled READ isTextModeEnabled WRITE setTextModeEnabled)
    Q_PROPERTY(bool isOpen READ isOpen CONSTANT)
    Q_PROPERTY(bool isReadable READ isReadable CONSTANT)
    Q_PROPERTY(bool isWritable READ isWritable CONSTANT)
    Q_PROPERTY(bool isSequential READ isSequential CONSTANT)

    Q_PROPERTY(int readChannelCount READ readChannelCount CONSTANT)
    Q_PROPERTY(int writeChannelCount READ writeChannelCount CONSTANT)
    Q_PROPERTY(int currentReadChannel READ currentReadChannel WRITE setCurrentReadChannel)
    Q_PROPERTY(int currentWriteChannel READ currentWriteChannel WRITE setCurrentWriteChannel)

    Q_PROPERTY(qint64 pos READ pos CONSTANT)
    Q_PROPERTY(qint64 size READ size CONSTANT)
    Q_PROPERTY(bool atEnd READ atEnd CONSTANT)
    Q_PROPERTY(bool canReadLine READ canReadLine CONSTANT)

    Q_PROPERTY(qint64 bytesAvailable READ bytesAvailable CONSTANT)
    Q_PROPERTY(qint64 bytesToWrite READ bytesToWrite CONSTANT)
public:
    enum OpenMode {
        NotOpen = 0x0000,
        ReadOnly = 0x0001,
        WriteOnly = 0x0002,
        ReadWrite = ReadOnly | WriteOnly,
        Append = 0x0004,
        Truncate = 0x0008,
        Text = 0x0010,
        Unbuffered = 0x0020,
        NewOnly = 0x0040,
        ExistingOnly = 0x0080
    };

    IODevice(QIODevice *source, QObject *parent = NULL);
    virtual ~IODevice() {}

    Q_INVOKABLE bool isValid() const { return d; }
    Q_INVOKABLE QIODevice* device() const { return d; }

    Q_INVOKABLE QIODevice::OpenMode openMode() const { return d ? d->openMode() : QIODevice::NotOpen; }

    Q_INVOKABLE void setTextModeEnabled(bool enabled) { if (d) d->setTextModeEnabled(enabled); }
    Q_INVOKABLE bool isTextModeEnabled() const { return d ? d->isTextModeEnabled() : false; }

    Q_INVOKABLE bool isOpen() const { return d ? d->isOpen() : false; }
    Q_INVOKABLE bool isReadable() const { return d ? d->isReadable() : false; }
    Q_INVOKABLE bool isWritable() const { return d ? d->isWritable() : false; }
    Q_INVOKABLE bool isSequential() const { return d ? d->isSequential() : false; }

    Q_INVOKABLE int readChannelCount() const;
    Q_INVOKABLE int writeChannelCount() const;
    Q_INVOKABLE int currentReadChannel() const;
    Q_INVOKABLE void setCurrentReadChannel(int channel);
    Q_INVOKABLE int currentWriteChannel() const;
    Q_INVOKABLE void setCurrentWriteChannel(int channel);

    Q_INVOKABLE bool open(IODevice::OpenMode mode);
    Q_INVOKABLE void close() { if (d) d->close(); }

    Q_INVOKABLE qint64 pos() const { return d ? d->pos() : 0; }
    Q_INVOKABLE qint64 size() const { return d ? d->size() : 0; }
    Q_INVOKABLE bool seek(qint64 pos) { return d ? d->seek(pos) : false; }
    Q_INVOKABLE bool atEnd() const { return d ? d->atEnd() : false; }
    Q_INVOKABLE bool reset() { return d ? d->reset() : false; }

    Q_INVOKABLE qint64 bytesAvailable() const { return d ? d->bytesAvailable() : 0; }
    Q_INVOKABLE qint64 bytesToWrite() const { return d ? d->bytesToWrite() : 0; }

    Q_INVOKABLE QByteArray read(qint64 maxlen) { return d ? d->read(maxlen) : QByteArray(); }
    Q_INVOKABLE QByteArray readAll() { return d ? d->readAll() : QByteArray(); }
    Q_INVOKABLE QByteArray readLine(qint64 maxlen = 0) { return d ? d->readLine(maxlen) : QByteArray(); }
    Q_INVOKABLE bool canReadLine() const { return d ? d->canReadLine() : false; }

    Q_INVOKABLE qint64 write(const QByteArray &data) { return d ? d->write(data) : 0; }
    Q_INVOKABLE QByteArray peek(qint64 maxlen) { return d ? d->peek(maxlen) : QByteArray(); }
    Q_INVOKABLE qint64 skip(qint64 maxSize);

    Q_INVOKABLE bool waitForReadyRead(int msecs) { return d ? d->waitForReadyRead(msecs) : false; }
    Q_INVOKABLE bool waitForBytesWritten(int msecs) { return d ? d->waitForBytesWritten(msecs) : false; }

    Q_INVOKABLE QString errorString() const { return d ? d->errorString() : QString(); }

signals:
    void isValidChanged();

    void readyRead();
    void channelReadyRead(int channel);
    void bytesWritten(qint64 bytes);
    void channelBytesWritten(int channel, qint64 bytes);
    void aboutToClose();
    void readChannelFinished();

protected:
    QPointer<QIODevice> d;
};

class KCL_EXPORT FileDevice : public IODevice
{
    Q_OBJECT
    Q_PROPERTY(QString fileName READ fileName WRITE setFileName NOTIFY fileNameChanged)
public:
    FileDevice(QFile *source, QObject *parent = NULL);
    FileDevice(const QString &fileName, QObject *parent = NULL);
    virtual ~FileDevice() {}

    Q_INVOKABLE QString fileName() const { return d ? f()->fileName() : QString(); }
    Q_INVOKABLE void setFileName(const QString &name);

    Q_INVOKABLE bool resize(qint64 sz) { return d ? f()->resize(sz) : false; }
    Q_INVOKABLE bool flush() { return d ? f()->flush() : false; }

    Q_INVOKABLE QFile::FileError error() const { return d ? f()->error() : QFile::NoError; }
    Q_INVOKABLE void unsetError() { if (d) f()->unsetError(); }

    Q_INVOKABLE int handle() const { return d ? f()->handle() : 0; }

signals:
    void fileNameChanged();

protected:
    inline QFile *f() const { return static_cast<QFile *>(d.data()); }
};

class KCL_EXPORT CryptographicHash : public QObject
{
    Q_OBJECT
    Q_ENUMS(Algorithm)
    Q_PROPERTY(QByteArray result READ result)
public:

#if KCL_QT5
    enum Algorithm {
        Md4,
        Md5,

        Sha1 = 2,
        Sha224,
        Sha256,
        Sha384,
        Sha512

#if QT_VERSION >= QT_VERSION_CHECK(5, 9, 2)
        ,
        Keccak_224 = 7,
        Keccak_256,
        Keccak_384,
        Keccak_512,
        RealSha3_224 = 11,
        RealSha3_256,
        RealSha3_384,
        RealSha3_512,
        Sha3_224 = RealSha3_224,
        Sha3_256 = RealSha3_256,
        Sha3_384 = RealSha3_384,
        Sha3_512 = RealSha3_512
#endif
    };
#else
    enum Algorithm {
        Md4,
        Md5,

        Sha1 = 2
    };
#endif

    CryptographicHash(Algorithm method) : QObject(0), hasher_((QCryptographicHash::Algorithm)method) {}
    ~CryptographicHash() {}

    Q_INVOKABLE void addData(const QByteArray &data) { hasher_.addData(data); }
#if KCL_QT5
    Q_INVOKABLE bool addData(IODevice *device) { return hasher_.addData(device->device()); }
#endif

    Q_INVOKABLE QByteArray result() const { return hasher_.result(); }
private:
    QCryptographicHash hasher_;
};

class KCL_EXPORT FileInfo : public QObject, public QFileInfo
{
    Q_OBJECT
    Q_ENUMS(FileTime)
    Q_PROPERTY(QString filePath READ filePath CONSTANT);
    Q_PROPERTY(QString absoluteFilePath READ absoluteFilePath CONSTANT);
    Q_PROPERTY(QString canonicalFilePath READ canonicalFilePath CONSTANT);
    Q_PROPERTY(QString fileName READ fileName CONSTANT);
    Q_PROPERTY(QString baseName READ baseName CONSTANT);
    Q_PROPERTY(QString completeBaseName READ completeBaseName CONSTANT);
    Q_PROPERTY(QString suffix READ suffix CONSTANT);
    Q_PROPERTY(QString bundleName READ bundleName CONSTANT);
    Q_PROPERTY(QString completeSuffix READ completeSuffix CONSTANT);

    Q_PROPERTY(QString path READ path CONSTANT);
    Q_PROPERTY(QString absolutePath READ absolutePath CONSTANT);
    Q_PROPERTY(QString canonicalPath READ canonicalPath CONSTANT);

    Q_PROPERTY(bool isReadable READ isReadable CONSTANT);
    Q_PROPERTY(bool isWritable READ isWritable CONSTANT);
    Q_PROPERTY(bool isExecutable READ isExecutable CONSTANT);
    Q_PROPERTY(bool isHidden READ isHidden CONSTANT);
    Q_PROPERTY(bool isNativePath READ isNativePath CONSTANT);

    Q_PROPERTY(bool isRelative READ isRelative CONSTANT);
    Q_PROPERTY(bool isAbsolute READ isAbsolute CONSTANT);

    Q_PROPERTY(bool isFile READ isFile CONSTANT);
    Q_PROPERTY(bool isDir READ isDir CONSTANT);
    Q_PROPERTY(bool isSymLink READ isSymLink CONSTANT);
    Q_PROPERTY(bool isRoot READ isRoot CONSTANT);
    Q_PROPERTY(bool isBundle READ isBundle CONSTANT);

    Q_PROPERTY(QString symLinkTarget READ symLinkTarget CONSTANT);

    Q_PROPERTY(QString owner READ owner CONSTANT);
    Q_PROPERTY(uint ownerId READ ownerId CONSTANT);
    Q_PROPERTY(QString group READ group CONSTANT);
    Q_PROPERTY(uint groupId READ groupId CONSTANT);
    Q_PROPERTY(QFile::Permissions permissions READ permissions CONSTANT);

    Q_PROPERTY(qint64 size READ size CONSTANT);

    Q_PROPERTY(QDateTime created READ created CONSTANT);
    Q_PROPERTY(QDateTime birthTime READ birthTime CONSTANT);
    Q_PROPERTY(QDateTime metadataChangeTime READ metadataChangeTime CONSTANT);
    Q_PROPERTY(QDateTime lastModified READ lastModified CONSTANT);
    Q_PROPERTY(QDateTime lastRead READ lastRead CONSTANT);

public:
    explicit FileInfo(const QString &fileName);
    virtual ~FileInfo();

    Q_INVOKABLE void refresh() { QFileInfo::refresh(); }
    Q_INVOKABLE bool makeAbsolute() { return QFileInfo::makeAbsolute(); }

    enum FileTime {
        FileAccessTime,
        FileBirthTime,
        FileMetadataChangeTime,
        FileModificationTime
    };

    Q_INVOKABLE QDateTime fileTime(FileTime time);
    Q_INVOKABLE bool permission(QFile::Permissions permissions) const { return QFileInfo::permission(permissions); }

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    bool isNativePath() const;
#endif

#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
    QDateTime birthTime() const;
    QDateTime metadataChangeTime() const;
#endif
};

class KCL_EXPORT FileSystemUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString homeLocation READ homeLocation CONSTANT)
    Q_PROPERTY(QString documentsLocation READ documentsLocation CONSTANT)
    Q_PROPERTY(QString desktopLocation READ desktopLocation CONSTANT)
    Q_PROPERTY(QString picturesLocation READ picturesLocation CONSTANT)
    Q_PROPERTY(QString musicLocation READ musicLocation CONSTANT)
    Q_PROPERTY(QString moviesLocation READ moviesLocation CONSTANT)
    Q_PROPERTY(QString tempLocation READ tempLocation CONSTANT)
    Q_PROPERTY(QString dataLocation READ dataLocation CONSTANT)
    Q_PROPERTY(QString cacheLocation READ cacheLocation CONSTANT)
    Q_PROPERTY(QString applicationsLocation READ applicationsLocation CONSTANT)
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

    Q_INVOKABLE static QUrl urlFromString(const QString &url);
    Q_INVOKABLE static QString stringFromUrl(const QUrl &url);
    Q_INVOKABLE static QUrl urlFromLocalPath(const QString &path);
    Q_INVOKABLE static QString localPathFromUrl(const QUrl &url);

    Q_INVOKABLE static QString pathAppend(const QString &path1, const QString &path2);
    Q_INVOKABLE static QString makeRelativeFilePath(const QString &base, const QString &target);
    Q_INVOKABLE static QString absoluteFilePath(const QString &filePath);
    Q_INVOKABLE static QString canonicalFilePath(const QString &filePath);

    Q_INVOKABLE static QDateTime lastModified(const QString &path);
    Q_INVOKABLE static QDateTime created(const QString &path);

    Q_INVOKABLE static QString findNextParent(const QString &path);

    Q_INVOKABLE static qint64 fileSize(const QString &path);
    Q_INVOKABLE static QString formatFileSize(long long fileSize);

    Q_INVOKABLE static FileInfo *getFileInfo(const QString &fileName);
    Q_INVOKABLE static FileDevice* getFile(const QString &fileName);

    Q_INVOKABLE static QByteArray getContents(const QString &fileName);
    Q_INVOKABLE static bool putContents(const QString &fileName, const QByteArray &contents, bool append = false);

    Q_INVOKABLE static QString md5HashFile(const QString &fileName);
    Q_INVOKABLE static CryptographicHash *getHasher(CryptographicHash::Algorithm algorithm);

    Q_INVOKABLE static bool forceDirectory(const QString &dirName);
    Q_INVOKABLE static bool removeDirectoryRecursively(const QString &dirName);

    Q_INVOKABLE static bool cd(const QString &path);
    Q_INVOKABLE static QString pwd();
    Q_INVOKABLE static QStringList dirList(const QStringList &nameFilters, QDir::Filters filter = QDir::NoFilter, QDir::SortFlags sort = QDir::NoSort);
    Q_INVOKABLE static QStringList dirList(QDir::Filters filter = QDir::NoFilter, QDir::SortFlags sort = QDir::NoSort);

    Q_INVOKABLE static FileScanner *find(const QStringList &targetDirectories, const QStringList &nameFilters);

    Q_INVOKABLE static bool removeFile(const QString &fileName);
    Q_INVOKABLE static bool renameFile(const QString &oldFileName, const QString &newFileName);
    Q_INVOKABLE static bool copyFile(const QString &srcFileName, const QString &destFileName);
    Q_INVOKABLE static bool copyRecursively(const QString &srcFilePath, const QString &destFilePath, bool ignoreErrors = false, QStringList *filesWithErrors = NULL);

    Q_INVOKABLE static void syncToDisk();

    static QString homeLocation();
    static QString documentsLocation();
    static QString desktopLocation();
    static QString picturesLocation();
    static QString musicLocation();
    static QString moviesLocation();
    static QString tempLocation();
    static QString dataLocation();
    static QString cacheLocation();
    static QString applicationsLocation();

signals:

public slots:

};

#endif // FILESYSTEMUTILS_H
