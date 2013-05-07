#include "filesystemutils.h"
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

QVariantMap FileSystemUtils::getOpenFileName(const QString &caption, const QString &startWith, const QString &filter, bool resolveSymlinks)
{
    QString selectedFilter;
    QString fileName = QFileDialog::getOpenFileName(0, caption, startWith, filter, &selectedFilter, resolveSymlinks ? QFlags<QFileDialog::Option>(0) : QFileDialog::DontResolveSymlinks);

    QVariantMap map;
    map["fileName"] = fileName;
    map["selectedFilter"] = selectedFilter;
    return map;
}

QVariantMap FileSystemUtils::getSaveFileName(const QString &caption, const QString &startWith, const QString &filter, bool resolveSymlinks)
{
    QString selectedFilter;
    QString fileName = QFileDialog::getSaveFileName(0, caption, startWith, filter, &selectedFilter, resolveSymlinks ? QFlags<QFileDialog::Option>(0) : QFileDialog::DontResolveSymlinks);

    QVariantMap map;
    map["fileName"] = fileName;
    map["selectedFilter"] = selectedFilter;
    return map;
}

QString FileSystemUtils::getExistingDirectory(const QString &caption, const QString &dir, bool dirOnly, bool resolveSymlinks)
{
    return QDir::fromNativeSeparators(QFileDialog::getExistingDirectory(0, caption, dir,
                    (resolveSymlinks ? QFlags<QFileDialog::Option>(0) : QFileDialog::DontResolveSymlinks) |
                    (dirOnly ? QFileDialog::ShowDirsOnly : QFlags<QFileDialog::Option>(0)))
    );
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
