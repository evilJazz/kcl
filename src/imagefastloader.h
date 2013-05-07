#ifndef IMAGEFASTLOADER_H
#define IMAGEFASTLOADER_H

#include <QIODevice>
#include <QList>
#include <QSize>
#include <QImage>

class CachedImageSize
{
public:
    CachedImageSize() : width(0), height(0), offset(0), count(0), maskCount(0) {}
    qint32 width;
    qint32 height;
    qint32 offset;
    qint32 count;
    qint32 maskCount;
};

class DiskImageCache : public QObject
{
    Q_OBJECT
public:
    enum ImageCacheResult
    {
        NoError = 0,
        LoadError = 1,
        SaveError = 2,
        RequestedSizeNotInCache = 3,
        KeyInvalid = 4,
        KeyDoesNotExist = 5
    };

    DiskImageCache(QObject *parent = 0);

    QString cacheDirectory() const { return cacheDirectory_; }
    void setCacheDirectory(const QString &dirName) { cacheDirectory_ = dirName; }

    QList<QSize> &imageSizes() { return imageSizes_; }

    virtual bool isImageCached(const QString &key) const;

    virtual ImageCacheResult saveImage(const QString &key, QImage *srcImage);
    virtual ImageCacheResult saveImage(const QString &key, QIODevice *srcImageStream);
    virtual ImageCacheResult loadImage(const QString &key, QImage *dstImage, const QSize &requestedSize, bool returnExactSize = false, QSize *originalSize = NULL);

public:
    static bool saveCacheImages(QImage *srcImage, const QList<QSize> &sizes, QIODevice *dstStream);
    static QList<CachedImageSize> readCacheImageSizes(QIODevice *srcStream);
    static int findBestCacheImageMatch(const QList<CachedImageSize> &cacheImageSizes, const QSize &requestedSize);
    static ImageCacheResult loadCacheImage(QIODevice *srcStream, const CachedImageSize &imageSize, QImage *dstImage);
    static ImageCacheResult loadCacheImage(QIODevice *srcStream, const QSize &requestedSize, QImage *dstImage, bool returnExactSize = false, QSize *originalSize = NULL);

protected:
    virtual QString getFilenameForKey(const QString &key) const;

private:
    QString cacheDirectory_;
    QList<QSize> imageSizes_;
};

class ImageFastLoader : public DiskImageCache
{
    Q_OBJECT
public:
    ImageFastLoader(QObject *parent = 0) : DiskImageCache(parent) {}

    virtual DiskImageCache::ImageCacheResult loadImage(const QString &filename, QImage *dstImage, const QSize &requestedSize, bool returnExactSize = false, QSize *originalSize = NULL);
    virtual DiskImageCache::ImageCacheResult getImage(const QString &filename, QImage *dstImage, const QSize &requestedSize, bool returnExactSize = false, QSize *originalSize = NULL);

protected:
    virtual QString getFilenameForKey(const QString &key) const;
};

#endif // IMAGEFASTLOADER_H
