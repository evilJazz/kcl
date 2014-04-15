#ifndef GRAPHICSUTILS_H
#define GRAPHICSUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QSize>
#include <QSizeF>
#include <QRect>
#include <QRectF>

class KCL_EXPORT GraphicsUtils : public QObject
{
    Q_OBJECT
public:
    explicit GraphicsUtils(QObject *parent = 0);

    Q_INVOKABLE QSize scaledSize(QSize srcSize, const QSize &dstSize, Qt::AspectRatioMode mode = Qt::KeepAspectRatio);
    Q_INVOKABLE QSizeF scaledSize(QSizeF srcSize, const QSizeF &dstSize, Qt::AspectRatioMode mode = Qt::KeepAspectRatio);

    Q_INVOKABLE QSize scaleSize(const QSize &srcSize, qreal scale);
    Q_INVOKABLE QSizeF scaleSize(const QSizeF &srcSize, qreal scale);

    Q_INVOKABLE QRect scaleRect(const QRect &srcRect, qreal scale);
    Q_INVOKABLE QRectF scaleRect(const QRectF &srcRect, qreal scale);

signals:

public slots:

};

#endif // GRAPHICSUTILS_H
