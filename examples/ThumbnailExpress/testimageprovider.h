#ifndef TESTIMAGEPROVIDER_H
#define TESTIMAGEPROVIDER_H

#ifdef KCL_QTQUICK2
    #include <QQuickImageProvider>
#else
    #include <QDeclarativeImageProvider>
#endif

#include <QImage>
#include <QSize>

#include "KCL/sleepaid.h"

class TestImageProvider :
#ifdef KCL_QTQUICK2
    public QQuickImageProvider
#else
    public QDeclarativeImageProvider
#endif
{
public:
    explicit TestImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // TESTIMAGEPROVIDER_H
