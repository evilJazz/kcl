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
