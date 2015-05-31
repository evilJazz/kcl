#ifndef COOKIEJARINTERFACE_H
#define COOKIEJARINTERFACE_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QNetworkCookieJar>

#ifdef KCL_QTQUICK2
    #include <QtQuick>
#else
    #include <QtDeclarative>
#endif

class KCL_EXPORT CookieJarInterface : public QObject
{
    Q_OBJECT
public:
#ifdef KCL_QTQUICK2
    explicit CookieJarInterface(QQmlEngine *engine);
#else
    explicit CookieJarInterface(QDeclarativeEngine *engine);
#endif
    virtual ~CookieJarInterface();

    Q_INVOKABLE virtual QVariantList cookiesForUrl(const QUrl &url) const;
    Q_INVOKABLE virtual bool setCookiesFromUrl(const QVariantList &cookieList, const QUrl &url);

private:
#ifdef KCL_QTQUICK2
    QQmlEngine *engine_;
#else
    QDeclarativeEngine *engine_;
#endif

    QNetworkCookieJar *cookieJar() const;
};

#endif // COOKIEJARINTERFACE_H
