#include "KCL/base64imageprovider.h"

#include <QBuffer>
#include <QImageReader>

#ifdef KCL_QTQUICK2
    #define QDeclarativeImageProvider QQuickImageProvider
#endif


Base64ImageProvider::Base64ImageProvider() :
    QDeclarativeImageProvider(QDeclarativeImageProvider::Image)
{
}

QImage Base64ImageProvider::requestImage(const QString &id, QSize *size, const QSize &requestedSize)
{
    QByteArray imageData = QByteArray::fromBase64(id.toLatin1());
    QBuffer *imageDevice = new QBuffer(&imageData);
    imageDevice->open(QIODevice::ReadOnly);

    QImageReader reader;
    reader.setDecideFormatFromContent(true); // Important!
    reader.setDevice(imageDevice);
    QImage originalImage = reader.read();
    QSize originalSize = originalImage.size();
    imageDevice->close();

    QImage resultImage;
    if (originalSize == requestedSize)
    {
        if (size)
            *size = originalSize;

        resultImage = originalImage;
    }
    else
    {
        QSize fittedSize(originalSize);
        fittedSize.scale(requestedSize, Qt::KeepAspectRatio);
        *size = fittedSize;

        imageDevice->open(QIODevice::ReadOnly);
        reader.setDevice(imageDevice);
        reader.setScaledSize(fittedSize);
        resultImage = reader.read();
        imageDevice->close();
    }

    delete imageDevice;

    return resultImage;
}
