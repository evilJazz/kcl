#ifndef BASE64IMAGEPROVIDER_H
#define BASE64IMAGEPROVIDER_H

#include <QImage>
#include <QSize>

#ifdef KCL_QTQUICK2
    #include <QQuickImageProvider>
#else
    #include <QDeclarativeImageProvider>
#endif

class Base64ImageProvider :
#ifdef KCL_QTQUICK2
    public QQuickImageProvider
#else
    public QDeclarativeImageProvider
#endif
{
public:
    explicit Base64ImageProvider();
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);
};

#endif // BASE64IMAGEPROVIDER_H
