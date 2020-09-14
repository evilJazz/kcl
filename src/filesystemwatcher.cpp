#include "../include/KCL/filesystemwatcher.h"

FileSystemWatcher::FileSystemWatcher(QObject *parent) :
    QObject(parent),
    enabled_(false),
    d(NULL)
{
    setEnabled(true);
}

FileSystemWatcher::~FileSystemWatcher()
{
    setEnabled(false);
    clear();
}

void FileSystemWatcher::setEnabled(bool enabled)
{
    if (enabled != enabled_)
    {
        enabled_ = enabled;

        if (enabled_ && !d)
        {
            d = new QFileSystemWatcher(this);
            connect(d, SIGNAL(fileChanged(const QString &)), this, SIGNAL(fileChanged(const QString &)));
            connect(d, SIGNAL(directoryChanged(const QString &)), this, SIGNAL(directoryChanged(const QString &)));
            addPaths(pathsToWatch_);
        }
        else if (!enabled_ && d)
        {
            d->deleteLater();
            d = NULL;
        }

        emit enabledChanged();
    }
}

QStringList FileSystemWatcher::files() const
{
    return d ? d->files() : QStringList();
}

QStringList FileSystemWatcher::directories() const
{
    return d ? d->directories() : QStringList();
}

void FileSystemWatcher::clear()
{
    pathsToWatch_.clear();

    if (enabled())
    {
        setEnabled(false);
        setEnabled(true);
    }
}

bool FileSystemWatcher::addPath(const QString &file)
{
    bool result = true;
    pathsToWatch_.append(file);

    if (d)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        result = d->addPath(file);
#else
        d->addPath(file);
#endif
    }

    if (!updating())
        emit changed();

    return result;
}

QStringList FileSystemWatcher::addPaths(const QStringList &files)
{
    QStringList result;

    beginUpdate();

    if (d)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        result = d->addPaths(files);
#else
        d->addPaths(files);
#endif
    }

    endUpdate();

    emit changed();
    return result;
}

bool FileSystemWatcher::removePath(const QString &file)
{
    bool result = true;
    pathsToWatch_.removeOne(file);

    if (d)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        result = d->removePath(file);
#else
        d->removePath(file);
#endif
    }

    if (!updating())
        emit changed();

    return result;
}

QStringList FileSystemWatcher::removePaths(const QStringList &files)
{
    QStringList result;

    beginUpdate();

    if (d)
    {
#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)
        result = d->removePaths(files);
#else
        d->removePaths(files);
#endif
    }

    endUpdate();

    emit changed();
    return result;
}
