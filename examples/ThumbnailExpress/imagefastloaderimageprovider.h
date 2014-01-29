#ifndef IMAGEFASTLOADERIMAGEPROVIDER_H
#define IMAGEFASTLOADERIMAGEPROVIDER_H

#ifdef KCL_QTQUICK2
    #include <QQuickImageProvider>
#else
    #include <QDeclarativeImageProvider>
#endif

#include <QImage>
#include <QSize>

#include "KCL/imagefastloader.h"

class ImageFastLoaderImageProvider :
#ifdef KCL_QTQUICK2
    public QQuickImageProvider
#else
    public QDeclarativeImageProvider
#endif
{
public:
    explicit ImageFastLoaderImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

protected:
    KCL::ImageFastLoader fastLoader_;
};

#endif // IMAGEFASTLOADERIMAGEPROVIDER_H
