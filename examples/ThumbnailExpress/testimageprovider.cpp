#include "testimageprovider.h"

#ifdef KCL_QTQUICK2
    #define QDeclarativeImageProvider QQuickImageProvider
#endif

TestImageProvider::TestImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage TestImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    int width = 100;
    int height = 50;

    if (size)
        *size = QSize(width, height);

    QImage image(requestedSize.width() > 0 ? requestedSize.width() : width,
                 requestedSize.height() > 0 ? requestedSize.height() : height,
                 QImage::Format_ARGB32_Premultiplied);

    //image.fill(QColor(id).rgba());
    image.fill(QColor(qrand()).rgba());

    KCL::SleepAid::msleep(qrand() % 20);
    //SleepAid::msleep(20);

    return image;
}
