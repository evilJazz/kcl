#include "KCL/colorutils.h"

ColorUtils::ColorUtils(QObject *parent) :
    QObject(parent)
{
}

QVariantMap ColorUtils::colorComponents(const QColor &color)
{
    QVariantMap result;

    result.insert("isValid", color.isValid());
    result.insert("a", color.alpha());
    result.insert("r", color.red());
    result.insert("g", color.green());
    result.insert("b", color.blue());

    return result;
}

QString ColorUtils::colorName(const QColor &color)
{
    return QString().sprintf("#%08X", color.rgba());
}

QColor ColorUtils::parseColor(const QString &colorString)
{
    if (colorString.startsWith("#") && colorString.length() == 9)
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
