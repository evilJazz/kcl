#ifndef GRAPHICSUTILS_H
#define GRAPHICSUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QSize>
#include <QSizeF>
#include <QRect>
#include <QRectF>
#include <QVariant>

class KCL_EXPORT GraphicsUtils : public QObject
{
    Q_OBJECT
public:
    explicit GraphicsUtils(QObject *parent = 0);

    Q_INVOKABLE QSize scaledSize(QSize srcSize, const QSize &dstSize, int aspectMode = Qt::KeepAspectRatio);
    Q_INVOKABLE QSizeF scaledSize(QSizeF srcSize, const QSizeF &dstSize, int aspectMode = Qt::KeepAspectRatio);

    Q_INVOKABLE QSize scaleSize(const QSize &srcSize, qreal scale);
    Q_INVOKABLE QSizeF scaleSize(const QSizeF &srcSize, qreal scale);

    Q_INVOKABLE QRect scaleRect(const QRect &srcRect, qreal scale);
    Q_INVOKABLE QRectF scaleRect(const QRectF &srcRect, qreal scale);

    bool imageFromVariant(const QVariant &image, QImage *result = NULL);

    Q_INVOKABLE QVariant loadImage(const QString &fileName, const QSize &requestedSize = QSize(), bool returnExactSize = false);
    Q_INVOKABLE bool saveImage(const QVariant &image, const QString &fileName, const QString &format = QString::null, int quality = -1);

    Q_INVOKABLE QVariant scaleImage(const QVariant &srcImage, const QSize &dstSize, int aspectMode = Qt::IgnoreAspectRatio, int mode = Qt::FastTransformation);
    Q_INVOKABLE QVariant copyImage(const QVariant &srcImage, const QRect &srcRect);

    Q_INVOKABLE QVariant imageSize(const QVariant &srcImage);

signals:

public slots:

};

#endif // GRAPHICSUTILS_H
