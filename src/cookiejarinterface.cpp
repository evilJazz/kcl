#include "KCL/cookiejarinterface.h"

#ifdef KCL_QTQUICK2
CookieJarInterface::CookieJarInterface(QQmlEngine *engine) :
    QObject(engine),
    engine_(engine)
#else
#include <QNetworkAccessManager>
#include <QNetworkCookie>

CookieJarInterface::CookieJarInterface(QDeclarativeEngine *engine) :
    QObject(engine),
    engine_(engine)
#endif
{

}

CookieJarInterface::~CookieJarInterface()
{

}

QNetworkCookieJar *CookieJarInterface::cookieJar() const
{
    if (!engine_)
    {
        qWarning("No engine set for CookieJarInterface to work with.");
        return NULL;
    }

    if (!engine_->networkAccessManager())
    {
        qWarning("No network access manager set on engine.");
        return NULL;
    }

    if (!engine_->networkAccessManager()->cookieJar())
    {
        qWarning("No cookie jar set on network access manager.");
        return NULL;
    }

    return engine_->networkAccessManager()->cookieJar();
}

QVariantList CookieJarInterface::cookiesForUrl(const QUrl &url) const
{
    QNetworkCookieJar *jar = cookieJar();
    if (jar)
    {
        QVariantList result;

        QList<QNetworkCookie> cookieList = jar->cookiesForUrl(url);

        foreach (const QNetworkCookie &cookie, cookieList)
            result += QString::fromUtf8(cookie.toRawForm(QNetworkCookie::Full));

        return result;
    }
    else
        return QVariantList();

}

bool CookieJarInterface::setCookiesFromUrl(const QVariantList &cookieList, const QUrl &url)
{
    QNetworkCookieJar *jar = cookieJar();
    if (jar)
    {
        bool result = true;

        foreach (const QVariant &cookieVar, cookieList)
        {
            QByteArray cookieString = cookieVar.toByteArray();
            if (!cookieString.isEmpty())
            {
                QList<QNetworkCookie> cookieList = QNetworkCookie::parseCookies(cookieString);
                result = result && jar->setCookiesFromUrl(cookieList, url);
            }
        }

        return result;
    }
    else
        return false;
}

#if QT_VERSION >= QT_VERSION_CHECK(5, 0, 0)

bool CookieJarInterface::insertCookie(const QString &cookieString)
{
    QNetworkCookieJar *jar = cookieJar();
    if (jar)
    {
        QList<QNetworkCookie> cookieList = QNetworkCookie::parseCookies(cookieString.toUtf8());

        bool result = true;

        foreach (const QNetworkCookie &cookie, cookieList)
            result = result && jar->insertCookie(cookie);

        return result;
    }
    else
        return false;
}

bool CookieJarInterface::updateCookie(const QString &cookieString)
{
    QNetworkCookieJar *jar = cookieJar();
    if (jar)
    {
        QList<QNetworkCookie> cookieList = QNetworkCookie::parseCookies(cookieString.toUtf8());

        bool result = true;

        foreach (const QNetworkCookie &cookie, cookieList)
            result = result && jar->updateCookie(cookie);

        return result;
    }
    else
        return false;
}

bool CookieJarInterface::deleteCookie(const QString &cookieString)
{
    QNetworkCookieJar *jar = cookieJar();
    if (jar)
    {
        QList<QNetworkCookie> cookieList = QNetworkCookie::parseCookies(cookieString.toUtf8());

        bool result = true;

        foreach (const QNetworkCookie &cookie, cookieList)
            result = result && jar->deleteCookie(cookie);

        return result;
    }
    else
        return false;
}

#endif
