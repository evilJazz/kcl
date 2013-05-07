#include "filescanner.h"

#include <QThread>
#include <QThreadPool>
#include <QDirIterator>

#include "debug.h"

#include "filesystemutils.h"

/* FileScannerRunnable */

FileScannerRunnable::FileScannerRunnable(FileScanner *fileScanner) :
    QObject(NULL),
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
        if (targetDirectoryName.startsWith("qrc:/")) // if resource path that remove qrc as that is not enumerable...
            targetDirectoryName.remove(0, 3);

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
                if (fileScanner_->returnExtendedResults())
                {
                    QFileInfo fi(it.fileInfo());
                    QVariantMap result;
                    result.insert("fileName", currentFilename);
                    result.insert("fileSize", fi.size());
                    result.insert("lastModified", fi.lastModified());
                    result.insert("created", fi.created());
                    result.insert("lastRead", fi.lastRead());

                    if (hashSums && fi.size() < maxFileSizeForHashSums)
                        result.insert("md5sum", FileSystemUtils::md5HashFile(currentFilename));

                    results.append(result);
                }
                else
                    results.append(currentFilename);
            }
        }

        if (fileScanner_->stopped())
            break;
    }

    emit resultsAvailable(results);
}

/* FileScanner */

FileScanner::FileScanner(QObject *parent) :
    QObject(parent),
    runnable_(NULL),
    returnExtendedResults_(false),
    returnHashSums_(false),
    maxFileSizeForHashSums_(30 * 1024 * 1024),
    useWorkerThread_(true),
    stop_(false)
{
}

FileScanner::~FileScanner()
{
    if (running())
        stop();

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
        delete runnableInstance; // in any case we need to delete the runnable manually!!
    }

    return true;
}

void FileScanner::setTargetDirectories(const QStringList targetDirectories)
{
    if (targetDirectories != targetDirectories_)
    {
        targetDirectories_ = targetDirectories;
        emit targetDirectoriesChanged();
    }
}

void FileScanner::setFilters(const QStringList filters)
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

void FileScanner::stop()
{
    stop_ = true;
}

void FileScanner::handleRunnableResultsAvailable(const QVariantList &list)
{
    runnable_ = NULL;
    stop_ = false;
    results_ = list;
    emit resultsAvailable();
}
