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

#ifndef FILESCANNER_H
#define FILESCANNER_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QRunnable>

class FileScanner;

class KCL_EXPORT FileScannerRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    FileScannerRunnable(FileScanner *fileScanner);
    virtual ~FileScannerRunnable();

signals:
    void resultsAvailable(const QVariantList &list);

protected:
    friend class FileScanner;
    void run();

private:
    FileScanner *fileScanner_;
};

class KCL_EXPORT FileScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList targetDirectories READ targetDirectories WRITE setTargetDirectories NOTIFY targetDirectoriesChanged)
    Q_PROPERTY(QStringList filters READ filters WRITE setFilters NOTIFY filtersChanged)
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsAvailable)
    Q_PROPERTY(bool returnExtendedResults READ returnExtendedResults WRITE setReturnExtendedResults NOTIFY returnExtendedResultsChanged)
    Q_PROPERTY(bool returnHashSums READ returnHashSums WRITE setReturnHashSums NOTIFY returnHashSumsChanged)
    Q_PROPERTY(int maxFileSizeForHashSums READ maxFileSizeForHashSums WRITE setMaxFileSizeForHashSums NOTIFY maxFileSizeForHashSumsChanged)
    Q_PROPERTY(bool useWorkerThread READ useWorkerThread WRITE setUseWorkerThread NOTIFY useWorkerThreadChanged)
    Q_PROPERTY(bool lowPriorityScanning READ lowPriorityScanning WRITE setLowPriorityScanning NOTIFY lowPriorityScanningChanged)
    Q_PROPERTY(bool running READ running NOTIFY resultsAvailable)
public:
    explicit FileScanner(QObject *parent = 0);
    virtual ~FileScanner();

    Q_INVOKABLE bool startScan();

    void setTargetDirectories(const QStringList targetDirectories);
    QStringList targetDirectories() const { return targetDirectories_; }

    void setFilters(const QStringList filters);
    QStringList filters() const { return filters_; }

    QVariantList results() const { return results_; }

    void setReturnExtendedResults(bool value);
    bool returnExtendedResults() const { return returnExtendedResults_; }

    void setReturnHashSums(bool value);
    bool returnHashSums() const { return returnHashSums_; }

    void setMaxFileSizeForHashSums(int fileSize);
    int maxFileSizeForHashSums() const { return maxFileSizeForHashSums_; }

    void setUseWorkerThread(bool value);
    bool useWorkerThread() const { return useWorkerThread_; }

    void setLowPriorityScanning(bool value);
    bool lowPriorityScanning() const { return lowPriorityScanning_; }

    bool running() const { return runnable_ != NULL; }

    Q_INVOKABLE void stop();
    bool stopped() const { return stop_; }

signals:
    void targetDirectoriesChanged();
    void filtersChanged();
    void resultsAvailable();
    void returnExtendedResultsChanged();
    void returnHashSumsChanged();
    void maxFileSizeForHashSumsChanged();
    void useWorkerThreadChanged();
    void lowPriorityScanningChanged();

private slots:
    void handleRunnableResultsAvailable(const QVariantList &list);

private:
    QStringList targetDirectories_;
    QStringList filters_;
    QVariantList results_;
    bool returnExtendedResults_;
    bool returnHashSums_;
    int maxFileSizeForHashSums_;
    bool useWorkerThread_;
    bool lowPriorityScanning_;

    bool stop_;

    FileScannerRunnable *runnable_;
};

#endif // FILESCANNER_H
