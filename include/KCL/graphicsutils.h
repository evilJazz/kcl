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

    Q_INVOKABLE static QSize scaledSize(QSize srcSize, const QSize &dstSize, int aspectMode = Qt::KeepAspectRatio);
    Q_INVOKABLE static QSizeF scaledSize(QSizeF srcSize, const QSizeF &dstSize, int aspectMode = Qt::KeepAspectRatio);

    Q_INVOKABLE static QSize scaleSize(const QSize &srcSize, qreal scale);
    Q_INVOKABLE static QSizeF scaleSize(const QSizeF &srcSize, qreal scale);

    Q_INVOKABLE static QRect scaleRect(const QRect &srcRect, qreal scale);
    Q_INVOKABLE static QRectF scaleRect(const QRectF &srcRect, qreal scale);

signals:

public slots:

};

#endif // GRAPHICSUTILS_H
