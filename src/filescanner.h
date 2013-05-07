#ifndef FILESCANNER_H
#define FILESCANNER_H

#include <QObject>
#include <QStringList>
#include <QVariant>
#include <QRunnable>

class FileScanner;

class FileScannerRunnable : public QObject, public QRunnable
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

class FileScanner : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList targetDirectories READ targetDirectories WRITE setTargetDirectories NOTIFY targetDirectoriesChanged)
    Q_PROPERTY(QStringList filters READ filters WRITE setFilters NOTIFY filtersChanged)
    Q_PROPERTY(QVariantList results READ results NOTIFY resultsAvailable)
    Q_PROPERTY(bool returnExtendedResults READ returnExtendedResults WRITE setReturnExtendedResults NOTIFY returnExtendedResultsChanged)
    Q_PROPERTY(bool returnHashSums READ returnHashSums WRITE setReturnHashSums NOTIFY returnHashSumsChanged)
    Q_PROPERTY(int maxFileSizeForHashSums READ maxFileSizeForHashSums WRITE setMaxFileSizeForHashSums NOTIFY maxFileSizeForHashSumsChanged)
    Q_PROPERTY(bool useWorkerThread READ useWorkerThread WRITE setUseWorkerThread NOTIFY useWorkerThreadChanged)
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

    bool stop_;

    FileScannerRunnable *runnable_;
};

#endif // FILESCANNER_H
