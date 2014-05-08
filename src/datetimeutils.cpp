#include "KCL/datetimeutils.h"

DateTimeUtils::DateTimeUtils(QObject *parent) :
    QObject(parent)
{
}

QDateTime DateTimeUtils::parse(const QString &dateTimeString, int format)
{
    return QDateTime::fromString(dateTimeString, (Qt::DateFormat)format);
}

QDateTime DateTimeUtils::parse(const QString &dateTimeString, const QString &format)
{
    return QDateTime::fromString(dateTimeString, format);
}
