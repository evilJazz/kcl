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

#ifndef BINARYFILEDOWNLOADER_H
#define BINARYFILEDOWNLOADER_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QByteArray>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>

class RequestCapturedData
{
public:
    QString method;
    QByteArray rawData;
};

class KCL_EXPORT BinaryFileDownloader : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QByteArray downloadedData READ downloadedData NOTIFY downloaded)
    Q_PROPERTY(int errorCode READ errorCode NOTIFY error)
    Q_PROPERTY(QString errorText READ errorText NOTIFY error)
    Q_PROPERTY(bool autoDelete READ autoDelete WRITE setAutoDelete NOTIFY autoDeleteChanged)
public:
    explicit BinaryFileDownloader(QObject *parent = NULL);
    virtual ~BinaryFileDownloader();

    Q_INVOKABLE void download(QString url) { get(url); }

    Q_INVOKABLE void get(QString url);
    Q_INVOKABLE void post(QString url, const QByteArray &rawData);

    Q_INVOKABLE void setRequestHeader(const QByteArray &key, const QByteArray &value);
    Q_INVOKABLE void clearRequestHeaders();

    Q_INVOKABLE QByteArray downloadedData() const { return downloadedData_; }

    int errorCode() const { return errorCode_; }
    QString errorText() const { return errorText_; }

    bool autoDelete() const;
    void setAutoDelete(bool value);

    QNetworkAccessManager *networkAccessManager();
    void setNetworkAccessManager(QNetworkAccessManager *networkAccessManager);

signals:
    void downloaded(QByteArray data);
    void error(int errorCode, const QString &errorText);
    void autoDeleteChanged();

private slots:
    void fileDownloaded();

private:
    QNetworkAccessManager *manager_;
    QByteArray downloadedData_;
    int errorCode_;
    QString errorText_;
    bool autoDelete_;

    QHash<QNetworkReply *, RequestCapturedData> requestData_;

    QMultiMap<QByteArray, QByteArray> requestHeaders_;

    void setHeadersOnRequest(QNetworkRequest *request);
};

#endif // BINARYFILEDOWNLOADER_H
