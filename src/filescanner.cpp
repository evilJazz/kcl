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

#include "KCL/filescanner.h"

#include <QThread>
#include <QThreadPool>
#include <QDirIterator>
#include <QDateTime>

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

#include "KCL/filesystemutils.h"

#include <QCoreApplication>

/* FileScannerRunnable */

FileScannerRunnable::FileScannerRunnable(FileScanner *fileScanner) :
    QObject(NULL),
    QRunnable(),
    fileScanner_(fileScanner)
{
}

FileScannerRunnable::~FileScannerRunnable()
{
}

void FileScannerRunnable::run()
{
    DGUARDMETHODTIMED;
    //moveToThread(QThread::currentThread());

    QThread::Priority oldPrio = QThread::currentThread()->priority();

    if (fileScanner_->lowPriorityScanning() && QThread::currentThread() != qApp->thread())
        QThread::currentThread()->setPriority(QThread::LowestPriority);

    // Prepare...
    QStringList filterStrings = fileScanner_->filters();
    QStringList targetDirectories = fileScanner_->targetDirectories();

    bool hashSums = fileScanner_->returnHashSums();
    int maxFileSizeForHashSums = fileScanner_->maxFileSizeForHashSums();

    QString currentFilename;
    QList<QRegExp> filters;

    foreach (QString filter, filterStrings)
        filters.append(QRegExp(filter, Qt::CaseInsensitive));

    // Execute...
    QVariantList results;
    bool addItem;

    foreach (QString targetDirectoryName, targetDirectories)
    {
        QString prefix = "";
        bool convertBackToUrl = false;

        if (targetDirectoryName.startsWith("qrc:/")) // if resource path that remove qrc as that is not enumerable...
        {
            prefix = "qrc";
            targetDirectoryName.remove(0, 3);
        }
        else if (targetDirectoryName.startsWith("file:"))
        {
            convertBackToUrl = true;
            targetDirectoryName = FileSystemUtils::localPathFromUrl(targetDirectoryName);
        }

        QDirIterator it(targetDirectoryName, QDir::Files | QDir::NoSymLinks, QDirIterator::Subdirectories);

        while (it.hasNext() && !fileScanner_->stopped())
        {
            currentFilename = it.next();
            addItem = false;

            if (filters.count() > 0)
            {
                foreach (QRegExp filter, filters)
                    if (currentFilename.contains(filter))
                    {
                        addItem = true;
                        break;
                    }
            }
            else
                addItem = true;

            if (addItem)
            {
                QString formattedFilename = prefix + currentFilename;

                if (convertBackToUrl)
                    formattedFilename = FileSystemUtils::urlFromLocalPath(formattedFilename).toString();

                if (fileScanner_->returnExtendedResults())
                {
                    QFileInfo fi(it.fileInfo());
                    QVariantMap result;
                    result.insert("fileName", formattedFilename);
                    result.insert("fileSize", fi.size());
                    result.insert("lastModified", fi.lastModified());
                    result.insert("created", fi.created());
                    result.insert("lastRead", fi.lastRead());

                    if (hashSums && fi.size() < maxFileSizeForHashSums)
                        result.insert("md5sum", FileSystemUtils::md5HashFile(currentFilename));

                    results.append(result);
                }
                else
                    results.append(formattedFilename);
            }

            if (fileScanner_->lowPriorityScanning())
                QThread::yieldCurrentThread();
        }

        if (fileScanner_->stopped())
            break;
    }

    if (QThread::currentThread()->priority() != oldPrio)
        QThread::currentThread()->setPriority(oldPrio); // Set old prio that was set by QThreadPool

    emit resultsAvailable(results);
}

/* FileScanner */

FileScanner::FileScanner(QObject *parent) :
    QObject(parent),
    returnExtendedResults_(false),
    returnHashSums_(false),
    maxFileSizeForHashSums_(30 * 1024 * 1024),
    useWorkerThread_(true),
    lowPriorityScanning_(false),
    stop_(false),
    runnable_(NULL)
{
    DGUARDMETHODTIMED;
}

FileScanner::~FileScanner()
{
    DGUARDMETHODTIMED;

    if (running())
        stop();

    if (useWorkerThread_ && running())
        QThreadPool::globalInstance()->waitForDone();
}

bool FileScanner::startScan()
{
    DGUARDMETHODTIMED;

    if (running())
        return false;

    stop_ = false;
    FileScannerRunnable *runnableInstance = new FileScannerRunnable(this);
    runnable_ = runnableInstance;
    connect(runnable_, SIGNAL(resultsAvailable(QVariantList)), this, SLOT(handleRunnableResultsAvailable(QVariantList)));

    if (useWorkerThread_)
        QThreadPool::globalInstance()->start(runnable_);
    else
    {
        runnable_->run();
        // runnable_ is reset in handleRunnableResultsAvailable slot...
        delete runnable_; // in any case we need to delete the runnable manually!!
        runnable_ = NULL;
    }

    return true;
}

void FileScanner::setTargetDirectories(const QStringList &targetDirectories)
{
    if (targetDirectories != targetDirectories_)
    {
        targetDirectories_ = targetDirectories;
        emit targetDirectoriesChanged();
    }
}

void FileScanner::setFilters(const QStringList &filters)
{
    if (filters != filters_)
    {
        filters_ = filters;
        emit filtersChanged();
    }
}

void FileScanner::setReturnExtendedResults(bool value)
{
    if (value != returnExtendedResults_)
    {
        returnExtendedResults_ = value;
        emit returnExtendedResultsChanged();
    }
}

void FileScanner::setReturnHashSums(bool value)
{
    if (value != returnHashSums_)
    {
        returnHashSums_ = value;
        emit returnHashSumsChanged();
    }
}

void FileScanner::setMaxFileSizeForHashSums(int fileSize)
{
    if (fileSize != maxFileSizeForHashSums_)
    {
        maxFileSizeForHashSums_ = fileSize;
        emit maxFileSizeForHashSumsChanged();
    }
}

void FileScanner::setUseWorkerThread(bool value)
{
    if (value != useWorkerThread_)
    {
        useWorkerThread_ = value;
        emit useWorkerThreadChanged();
    }
}

void FileScanner::setLowPriorityScanning(bool value)
{
    if (value != lowPriorityScanning_)
    {
        lowPriorityScanning_ = value;
        emit lowPriorityScanningChanged();
    }
}

void FileScanner::stop()
{
    stop_ = true;
}

void FileScanner::handleRunnableResultsAvailable(const QVariantList &list)
{
    DGUARDMETHODTIMED;
    runnable_ = NULL;
    stop_ = false;
    results_ = list;
    emit resultsAvailable();
}
