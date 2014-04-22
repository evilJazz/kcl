#include "KCL/graphicsutils.h"

#include <QImage>
#include <QImageReader>

GraphicsUtils::GraphicsUtils(QObject *parent) :
    QObject(parent)
{
}

QSize GraphicsUtils::scaledSize(QSize srcSize, const QSize &dstSize, int aspectMode)
{
    srcSize.scale(dstSize, (Qt::AspectRatioMode)aspectMode);
    return srcSize;
}

QSizeF GraphicsUtils::scaledSize(QSizeF srcSize, const QSizeF &dstSize, int aspectMode)
{
    srcSize.scale(dstSize, (Qt::AspectRatioMode)aspectMode);
    return srcSize;
}

QSize GraphicsUtils::scaleSize(const QSize &srcSize, qreal scale)
{
    return srcSize * scale;
}

QSizeF GraphicsUtils::scaleSize(const QSizeF &srcSize, qreal scale)
{
    return srcSize * scale;
}

QRect GraphicsUtils::scaleRect(const QRect &srcRect, qreal scale)
{
    return QRect(srcRect.x() * scale, srcRect.y() * scale, srcRect.width() * scale, srcRect.height() * scale);
}

QRectF GraphicsUtils::scaleRect(const QRectF &srcRect, qreal scale)
{
    return QRect(srcRect.x() * scale, srcRect.y() * scale, srcRect.width() * scale, srcRect.height() * scale);
}

bool GraphicsUtils::imageFromVariant(const QVariant &image, QImage *result)
{
    if (image.canConvert<QImage>())
    {
        if (result)
            *result = image.value<QImage>();

        return true;
    }

    return false;
}

QVariant GraphicsUtils::loadImage(const QString &fileName, const QSize &requestedSize, bool returnExactSize)
{
    QImageReader reader;
    reader.setDecideFormatFromContent(true); // Important!
    reader.setFileName(fileName);

    if (returnExactSize)
    {
        QSize fittedSize(reader.size());
        fittedSize.scale(requestedSize, Qt::KeepAspectRatio);
        reader.setScaledSize(fittedSize);
    }

    QImage result;
    if (!reader.read(&result))
        return QVariant(false);

    return result;
}

bool GraphicsUtils::saveImage(const QVariant &image, const QString &fileName, const QString &format, int quality)
{
    QImage img;
    if (imageFromVariant(image, &img) && !img.isNull())
        return img.save(fileName, format.isNull() ? 0 : format.toLatin1().constData(), quality);

    return false;
}

QVariant GraphicsUtils::scaleImage(const QVariant &srcImage, const QSize &dstSize, int aspectMode, int mode)
{
    QImage img;
    if (imageFromVariant(srcImage, &img) && !img.isNull())
        return img.scaled(dstSize, (Qt::AspectRatioMode)aspectMode, (Qt::TransformationMode)mode);

    return QVariant(false);
}

QVariant GraphicsUtils::copyImage(const QVariant &srcImage, const QRect &srcRect)
{
    QImage img;
    if (imageFromVariant(srcImage, &img) && !img.isNull())
        return img.copy(srcRect);

    return QVariant(false);
}

QVariant GraphicsUtils::imageSize(const QVariant &srcImage)
{
    QImage img;
    if (imageFromVariant(srcImage, &img) && !img.isNull())
        return img.size();

    return QSize(0, 0);
}
