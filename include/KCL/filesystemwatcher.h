#ifndef FILESYSTEMWATCHER_H
#define FILESYSTEMWATCHER_H

#include <QObject>
#include <QStringList>
#include <QFileSystemWatcher>
#include <KCL/updatelocker.h>

class FileSystemWatcher : public QObject, private UpdateLocker
{
    Q_OBJECT
    Q_PROPERTY(QStringList files READ files NOTIFY changed)
    Q_PROPERTY(QStringList directories READ directories NOTIFY changed)

    Q_PROPERTY(QStringList pathsToWatch READ pathsToWatch NOTIFY changed)

    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)
public:
    FileSystemWatcher(QObject *parent = NULL);
    ~FileSystemWatcher();

    bool enabled() const { return enabled_; }
    void setEnabled(bool enabled);

    QStringList pathsToWatch() const { return pathsToWatch_; }

    QStringList files() const;
    QStringList directories() const;

    Q_INVOKABLE void clear();

    Q_INVOKABLE bool addPath(const QString &file);
    Q_INVOKABLE QStringList addPaths(const QStringList &files);
    Q_INVOKABLE bool removePath(const QString &file);
    Q_INVOKABLE QStringList removePaths(const QStringList &files);

signals:
    void changed();
    void enabledChanged();

    void fileChanged(const QString &path);
    void directoryChanged(const QString &path);

private:
    bool enabled_;
    QFileSystemWatcher *d;

    QStringList pathsToWatch_;
};

#endif // FILESYSTEMWATCHER_H
