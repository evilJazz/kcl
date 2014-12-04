#include "KCL/colorutils.h"

#ifdef KCL_WIDGETS
#include <QPixmap>
#include <QImage>
#include <QScreen>
#include <QGuiApplication>
#include <QApplication>
#include <QDesktopWidget>
#endif

ColorUtils::ColorUtils(QObject *parent) :
    QObject(parent)
{
}

QVariantMap ColorUtils::colorComponents(const QColor &color, bool asFloat)
{
    QVariantMap result;

    result.insert("isValid", color.isValid());

    if (asFloat)
    {
        result.insert("a", color.alphaF());
        result.insert("r", color.redF());
        result.insert("g", color.greenF());
        result.insert("b", color.blueF());
    }
    else
    {
        result.insert("a", color.alpha());
        result.insert("r", color.red());
        result.insert("g", color.green());
        result.insert("b", color.blue());
    }

    return result;
}

QString ColorUtils::colorName(const QColor &color)
{
    return QString().sprintf("#%08X", color.rgba());
}

QColor ColorUtils::parseColor(const QString &colorString)
{
    if (colorString.startsWith("#") && colorString.length() == 7)
    {
        int red = ("0x" + colorString.mid(1, 2)).toUShort(0, 16);
        int green = ("0x" + colorString.mid(3, 2)).toUShort(0, 16);
        int blue = ("0x" + colorString.mid(5, 2)).toUShort(0, 16);

        return QColor(red, green, blue);
    }
    else if (colorString.startsWith("#") && colorString.length() == 9)
    {
        int alpha = ("0x" + colorString.mid(1, 2)).toUShort(0, 16);
        int red = ("0x" + colorString.mid(3, 2)).toUShort(0, 16);
        int green = ("0x" + colorString.mid(5, 2)).toUShort(0, 16);
        int blue = ("0x" + colorString.mid(7, 2)).toUShort(0, 16);

        return QColor(red, green, blue, alpha);
    }
    else
        return QColor(colorString);
}

QColor ColorUtils::setAlpha(const QColor &color, QVariant alpha)
{
    QColor copy = color;

    if (alpha.type() == QVariant::Double)
        copy.setAlphaF(alpha.toDouble());
    else
        copy.setAlpha(alpha.toInt());

    return copy;
}

QColor ColorUtils::grabColorFromScreen(const QPoint &screenPos)
{
#ifdef KCL_WIDGETS
    QPoint p = screenPos;
    if (p == QPoint(INT_MAX, INT_MAX))
        p = QCursor::pos();

    const QDesktopWidget *desktop = QApplication::desktop();
    const QPixmap pixmap = QGuiApplication::screens().at(desktop->screenNumber())->grabWindow(desktop->winId(), p.x(), p.y(), 1, 1);
    QImage i = pixmap.toImage();

    return i.pixel(0, 0);
#else
    qWarning("ColorUtils::grabColorFromScreen is not implemented.");
    return QColor();
#endif
}
