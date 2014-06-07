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

void BinaryFileDownloader::download(QString url)
{
    QNetworkRequest request(QUrl::fromEncoded(url.toUtf8()));
    //request.setRawHeader("User-Agent", "Mozilla/5.0 (X11; Linux x86_64; rv:7.0.1) Gecko/20100101 Firefox/7.0.1");
    manager()->get(request);
}

void BinaryFileDownloader::fileDownloaded(QNetworkReply* reply)
{
    //qDebug("reply->url(): %s", (const char *)reply->url().toString().toUtf8());
    //qDebug("reply->error(): %d", reply->error());

    if (reply->error() != 200 && reply->error() != 0)
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

        connect(manager_, SIGNAL(finished(QNetworkReply*)), this, SLOT(fileDownloaded(QNetworkReply*)));
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
