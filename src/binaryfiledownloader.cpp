/***************************************************************************
 *   Copyright (C) 2011-2013 Andre Beckedorf                               *
 * 			     <evilJazz _AT_ katastrophos _DOT_ net>                    *
 *                                                                         *
 *   This library is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License version   *
 *   2.1 as published by the Free Software Foundation.                     *
 *                                                                         *
 *   This library is distributed in the hope that it will be useful, but   *
 *   WITHOUT ANY WARRANTY; without even the implied warranty of            *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU     *
 *   Lesser General Public License for more details.                       *
 *                                                                         *
 *   You should have received a copy of the GNU Lesser General Public      *
 *   License along with this library; if not, write to the Free Software   *
 *   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA         *
 *   02110-1301  USA                                                       *
 *                                                                         *
 *   Alternatively, this file is available under the Mozilla Public        *
 *   License Version 1.1.  You may obtain a copy of the License at         *
 *   http://www.mozilla.org/MPL/                                           *
 ***************************************************************************/

#include "KCL/binaryfiledownloader.h"

#ifdef KCL_QTQUICK2
    #include <QQmlEngine>
    #include <QQmlContext>
    #define KCLDeclarativeEngine QQmlEngine
    #define KCLDeclarativeContext QQmlContext
#else
    #include <QDeclarativeEngine>
    #include <QDeclarativeContext>
    #define KCLDeclarativeEngine QDeclarativeEngine
    #define KCLDeclarativeContext QDeclarativeContext
#endif

BinaryFileDownloader::BinaryFileDownloader() :
    QObject(),
    manager_(NULL),
    errorCode_(0),
    autoDelete_(false)
{
}

BinaryFileDownloader::~BinaryFileDownloader()
{
}

void BinaryFileDownloader::get(QString url)
{
    QNetworkRequest request(QUrl::fromEncoded(url.toUtf8()));
    setHeadersOnRequest(&request);

    QNetworkReply *reply = manager()->get(request);
    connect(reply, SIGNAL(finished()), this, SLOT(fileDownloaded()));

    RequestCapturedData data;
    data.method = "GET";

    requestData_.insert(reply, data);
}

void BinaryFileDownloader::post(QString url, const QByteArray &rawData)
{
    QNetworkRequest request(QUrl::fromEncoded(url.toUtf8()));
    setHeadersOnRequest(&request);

    QNetworkReply *reply = manager()->post(request, rawData);
    connect(reply, SIGNAL(finished()), this, SLOT(fileDownloaded()));

    RequestCapturedData data;
    data.method = "POST";
    data.rawData = rawData;

    requestData_.insert(reply, data);
}

void BinaryFileDownloader::setHeadersOnRequest(QNetworkRequest *request)
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

void BinaryFileDownloader::setRequestHeader(const QByteArray &key, const QByteArray &value)
{
    requestHeaders_.insertMulti(key, value);
}

void BinaryFileDownloader::clearRequestHeaders()
{
    requestHeaders_.clear();
}

void BinaryFileDownloader::fileDownloaded()
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

        if (reply->error() != QNetworkReply::NoError)
        {
            errorCode_ = reply->error();
            errorText_ = reply->errorString();
            emit error(errorCode_, errorText_);
        }
        else
        {
            downloadedData_ = reply->readAll();
            emit downloaded(downloadedData_);
        }

        reply->deleteLater();
    }
    else
        emit error(-1, "Invalid reply received.");

    if (autoDelete_)
        deleteLater();
}

QNetworkAccessManager *BinaryFileDownloader::manager()
{
    if (!manager_)
    {
        KCLDeclarativeContext *context = KCLDeclarativeEngine::contextForObject(this);

        if (context)
            manager_ = context->engine()->networkAccessManager();
        else
            manager_ = new QNetworkAccessManager(this);

        //connect(manager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
    }

    return manager_;
}

bool BinaryFileDownloader::autoDelete() const
{
    return autoDelete_;
}

void BinaryFileDownloader::setAutoDelete(bool value)
{
    if(autoDelete_ != value)
    {
        autoDelete_ = value;
        emit autoDeleteChanged();
    }
}
