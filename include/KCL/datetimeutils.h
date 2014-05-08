#ifndef DATETIMEUTILS_H
#define DATETIMEUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QDateTime>

class DateTimeUtils : public QObject
{
    Q_OBJECT
public:
    explicit DateTimeUtils(QObject *parent = 0);

    // These methods for convenience because V4's Date.parse() is buggy or non-functional...
    Q_INVOKABLE static QDateTime parse(const QString &dateTimeString, int format = Qt::TextDate);
    Q_INVOKABLE static QDateTime parse(const QString &dateTimeString, const QString &format);
};

#endif // DATETIMEUTILS_H
