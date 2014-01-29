#ifndef COLORUTILS_H
#define COLORUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>
#include <QColor>

namespace KCL {

class KCL_EXPORT ColorUtils : public QObject
{
    Q_OBJECT
public:
    explicit ColorUtils(QObject *parent = 0);

    Q_INVOKABLE static QVariantMap colorComponents(const QColor &color);
    Q_INVOKABLE static QString colorName(const QColor &color);
    Q_INVOKABLE static QColor parseColor(const QString &colorString);

signals:

public slots:

};

}

#endif // COLORUTILS_H
