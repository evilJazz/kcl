#include "KCL/base64imageprovider.h"

#ifdef KCL_QTQUICK2
    #define QDeclarativeImageProvider QQuickImageProvider
#endif


Base64ImageProvider::Base64ImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage Base64ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    Q_UNUSED(size)
    Q_UNUSED(requestedSize)
    return QImage::fromData(QByteArray::fromBase64(id.toLatin1()));
}
