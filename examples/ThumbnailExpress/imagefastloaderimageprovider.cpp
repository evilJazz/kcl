#include "imagefastloaderimageprovider.h"

#include <QFile>

#include "KCL/filesystemutils.h"

#ifdef KCL_QTQUICK2
    #define QDeclarativeImageProvider QQuickImageProvider
#endif

ImageFastLoaderImageProvider::ImageFastLoaderImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
    fastLoader_.setCacheDirectory(KCL::FileSystemUtils::pathAppend(KCL::FileSystemUtils::cacheLocation(), "imgcache"));

    QList<QSize> cachedSizes;
    cachedSizes.append(QSize(32, 32));
    cachedSizes.append(QSize(64, 64));
    cachedSizes.append(QSize(128, 128));
    cachedSizes.append(QSize(180, 180));
    cachedSizes.append(QSize(400, 400));
    fastLoader_.imageSizes() = cachedSizes;
}

QImage ImageFastLoaderImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QImage resultImage;

    KCL::DiskImageCache::ImageCacheResult result = fastLoader_.getImage(id, &resultImage, requestedSize, true, size);

    return resultImage;
}
