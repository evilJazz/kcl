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

#ifndef WEBCALL_H
#define WEBCALL_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QPointer>

class RequestCapturedData
{
public:
    QString method;
    QByteArray rawData;
};

class KCL_EXPORT WebCall : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray replyData READ replyData NOTIFY success)
    Q_PROPERTY(int errorCode READ errorCode NOTIFY error)
    Q_PROPERTY(QString errorText READ errorText NOTIFY error)
    Q_PROPERTY(int statusCode READ statusCode NOTIFY attributesChanged)
    Q_PROPERTY(bool finished READ finished NOTIFY attributesChanged)
    Q_PROPERTY(QString reasonPhrase READ reasonPhrase NOTIFY attributesChanged)
    Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)
public:
    explicit WebCall(QObject *parent = NULL);
    virtual ~WebCall();

    Q_INVOKABLE void get(QString url);
    Q_INVOKABLE void post(QString url, const QByteArray &rawData);

    void get(QUrl url);
    void post(QUrl url, const QByteArray &rawData);

    Q_INVOKABLE void waitUntilFinished(int timeoutInMs = 0);

    Q_INVOKABLE void setRequestHeader(const QByteArray &key, const QByteArray &value);
    Q_INVOKABLE void clearRequestHeaders();

    Q_INVOKABLE QByteArray replyData() const { return replyData_; }

    int errorCode() const { return errorCode_; }
    QString errorText() const { return errorText_; }

    int statusCode() const { return statusCode_; }
    QString reasonPhrase() const { return reasonPhrase_; }

    bool finished() const { return finished_; }

    bool autoDelete() const;
    void setAutoDelete(bool value);

    QNetworkAccessManager *networkAccessManager();
    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

signals:
    void success(QByteArray data);
    void error(int errorCode, const QString &errorText);
    void autoDeleteChanged();
    void attributesChanged();
    void requestFinished();

private slots:
    void handleReplyFinished();

private:
    QNetworkAccessManager *manager_;
    QByteArray replyData_;
    bool finished_;
    QPointer<QNetworkReply> pendingRequest_;
    int errorCode_;
    QString errorText_;
    int statusCode_;
    QString reasonPhrase_;
    bool autoDelete_;

    QHash<QNetworkReply *, RequestCapturedData> requestData_;

    QMultiMap<QByteArray, QByteArray> requestHeaders_;

    void setHeadersOnRequest(QNetworkRequest *request);
    void resetState();
};

#endif // WEBCALL_H
