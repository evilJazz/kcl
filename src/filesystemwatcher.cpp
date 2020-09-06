#include "../include/KCL/filesystemwatcher.h"

FileSystemWatcher::FileSystemWatcher(QObject *parent) :
    QObject(parent),
    enabled_(false),
    d(nullptr)
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
            d = nullptr;
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
        result = d->addPath(file);

    if (!updating())
        emit changed();

    return result;
}

QStringList FileSystemWatcher::addPaths(const QStringList &files)
{
    QStringList result;

    beginUpdate();

    if (d)
        QStringList result = d->addPaths(files);

    endUpdate();

    emit changed();
    return result;
}

bool FileSystemWatcher::removePath(const QString &file)
{
    bool result = true;
    pathsToWatch_.removeOne(file);

    if (d)
        result = d->removePath(file);

    if (!updating())
        emit changed();

    return result;
}

QStringList FileSystemWatcher::removePaths(const QStringList &files)
{
    QStringList result;

    beginUpdate();

    if (d)
        result = d->removePaths(files);

    endUpdate();

    emit changed();
    return result;
}
