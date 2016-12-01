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

#include "KCL/webcall.h"

#ifdef KCL_QTQUICK2
    #include <QQmlEngine>
    #include <QQmlContext>
    #define KCLDeclarativeEngine QQmlEngine
    #define KCLDeclarativeContext QQmlContext
#elif KCL_QTQUICK1
    #include <QDeclarativeEngine>
    #include <QDeclarativeContext>
    #define KCLDeclarativeEngine QDeclarativeEngine
    #define KCLDeclarativeContext QDeclarativeContext
#endif

WebCall::WebCall(QObject *parent) :
    QObject(parent),
    manager_(NULL),
    errorCode_(0),
    autoDelete_(false)
{
}

WebCall::~WebCall()
{
}

void WebCall::get(QString url)
{
    get(QUrl::fromEncoded(url.toUtf8()));
}

void WebCall::post(QString url, const QByteArray &rawData)
{
    post(QUrl::fromEncoded(url.toUtf8()), rawData);
}

void WebCall::get(QUrl url)
{
    QNetworkRequest request(url);
    setHeadersOnRequest(&request);

    QNetworkReply *reply = networkAccessManager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(handleReplyFinished()));

    RequestCapturedData data;
    data.method = "GET";

    requestData_.insert(reply, data);
}

void WebCall::post(QUrl url, const QByteArray &rawData)
{
    QNetworkRequest request(url);
    setHeadersOnRequest(&request);

    QNetworkReply *reply = networkAccessManager()->post(request, rawData);
    connect(reply, SIGNAL(finished()), this, SLOT(handleReplyFinished()));

    RequestCapturedData data;
    data.method = "POST";
    data.rawData = rawData;

    requestData_.insert(reply, data);
}

void WebCall::setHeadersOnRequest(QNetworkRequest *request)
{
    foreach (const QByteArray &key, requestHeaders_.keys())
    {
        QList<QByteArray> values = requestHeaders_.values(key);

        QByteArray combinedValues;

        foreach (const QByteArray &value, values)
            combinedValues += value + ',';

        if (combinedValues.length() > 0)
            combinedValues.chop(1);

        request->setRawHeader(key, combinedValues);
    }
}

void WebCall::setRequestHeader(const QByteArray &key, const QByteArray &value)
{
    requestHeaders_.insertMulti(key, value);
}

void WebCall::clearRequestHeaders()
{
    requestHeaders_.clear();
}

void WebCall::handleReplyFinished()
{
    QNetworkReply *reply = qobject_cast<QNetworkReply *>(sender());

    if (reply)
    {
        QString possibleRedirectUrl = reply->attribute(QNetworkRequest::RedirectionTargetAttribute).toString();
        if (!possibleRedirectUrl.isEmpty())
        {
            RequestCapturedData data = requestData_[reply];

            if (data.method == "POST")
                post(possibleRedirectUrl, data.rawData);
            else
                get(possibleRedirectUrl);

            reply->deleteLater();

            return;
        }

        //qDebug("reply->url(): %s", (const char *)reply->url().toString().toUtf8());
        //qDebug("statuscode: %d", reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt());
        //qDebug("bytesAvailable: %d", reply->bytesAvailable());
        //qDebug("reply->error(): %d", reply->error());

        statusCode_ = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        reasonPhrase_ = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        emit attributesChanged();

        if (reply->error() != QNetworkReply::NoError)
        {
            errorCode_ = reply->error();
            errorText_ = reply->errorString();
            emit error(errorCode_, errorText_);
        }
        else
        {
            replyData_ = reply->readAll();
            emit success(replyData_);
        }

        reply->deleteLater();
    }
    else
        emit error(-1, "Invalid reply received.");

    if (autoDelete_)
        deleteLater();
}

QNetworkAccessManager *WebCall::networkAccessManager()
{
    if (!manager_)
    {
#if defined(KCL_QTQUICK1) || defined(KCL_QTQUICK2)
        KCLDeclarativeContext *context = KCLDeclarativeEngine::contextForObject(this);

        if (context)
            manager_ = context->engine()->networkAccessManager();
        else
#endif
            manager_ = new QNetworkAccessManager(this);

    }

    return manager_;
}

void WebCall::setNetworkAccessManager(QNetworkAccessManager *manager)
{
    manager_ = manager;
}

bool WebCall::autoDelete() const
{
    return autoDelete_;
}

void WebCall::setAutoDelete(bool value)
{
    if(autoDelete_ != value)
    {
        autoDelete_ = value;
        emit autoDeleteChanged();
    }
}
