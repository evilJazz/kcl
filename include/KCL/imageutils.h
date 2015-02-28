#ifndef IMAGEUTILS_H
#define IMAGEUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QSize>
#include <QRect>
#include <QVariant>
#include <QColor>

class KCL_EXPORT ImageUtils : public QObject
{
    Q_OBJECT
public:
    explicit ImageUtils(QObject *parent = 0);

    static bool checkParamsSameFormat(const QImage &srcImage, QImage &dstImage, bool modifyDst = true, bool zeroFillNew = false);

    static bool intensityToAlpha(const QImage &srcImage, QImage &dstImage);
    static bool minAlpha(const QImage &srcImage, QImage &dstImage);
    static bool maskedTransfer(const QImage &srcImage, QImage &dstImage, unsigned int mask);
    static bool applyAlpha(const QImage &srcImage, QImage &dstImage, unsigned int alpha);
    static bool hasAlphaValues(const QImage &srcImage, const QRect &srcRect = QRect());

    static bool convertToGrayscale(const QImage &srcImage, QImage &dstImage);

    static bool imageFromVariant(const QVariant &image, QImage *result = NULL);

    /* Image methods for use in JavaScript - all require QImage::Format_ARGB32 */

    Q_INVOKABLE static QVariant empty(const QSize &size);
    Q_INVOKABLE static QVariant load(const QString &fileName, const QSize &requestedSize = QSize(), bool returnExactSize = false);
    Q_INVOKABLE static bool save(const QVariant &image, const QString &fileName, const QString &format = QString::null, int quality = -1);

    Q_INVOKABLE static QVariant scaled(const QVariant &srcImage, const QSize &dstSize, int aspectMode = Qt::IgnoreAspectRatio, int mode = Qt::FastTransformation);
    Q_INVOKABLE static QVariant copy(const QVariant &srcImage, const QRect &srcRect);

    Q_INVOKABLE static QVariant fill(const QVariant &targetImage, const QColor &color);
    Q_INVOKABLE static QVariant invert(const QVariant &targetImage, bool invertAlphaToo = false);

    Q_INVOKABLE static QVariant maskedTransfer(const QVariant &srcImage, const QVariant &dstImage, quint32 mask);
    Q_INVOKABLE static QVariant intensityToAlpha(const QVariant &srcImage, const QVariant &dstImage);
    Q_INVOKABLE static QVariant minAlpha(const QVariant &srcImage, const QVariant &dstImage);
    Q_INVOKABLE static QVariant applyAlpha(const QVariant &srcImage, const QVariant &dstImage, quint32 alpha);

    Q_INVOKABLE static QVariant size(const QVariant &srcImage);

signals:

public slots:

};

#endif // IMAGEUTILS_H
