/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

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

#else

bool CookieJarInterface::insertCookie(const QString &cookieString)
{
    qWarning("CookieJarInterface::insertCookie not implemented on Qt 4");
}

bool CookieJarInterface::updateCookie(const QString &cookieString)
{
    qWarning("CookieJarInterface::updateCookie not implemented on Qt 4");
}

bool CookieJarInterface::deleteCookie(const QString &cookieString)
{
    qWarning("CookieJarInterface::deleteCookie not implemented on Qt 4");
}

#endif
