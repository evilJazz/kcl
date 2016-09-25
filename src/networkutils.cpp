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

#include "KCL/networkutils.h"
#include "KCL/performancedatamanager.h"

#include <QThreadPool>
#include <QTimer>

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

/* NetworkConfigurationRunnable */

class NetworkConfigurationRunnable : public QObject, public QRunnable
{
    Q_OBJECT
public:
    NetworkConfigurationRunnable() : QObject(NULL), QRunnable() { setAutoDelete(true); }
    virtual ~NetworkConfigurationRunnable() {}

signals:
    void done(QNetworkConfigurationManager *result);

protected:
    void run()
    {
        // Depending on the platform, getting an instance of the
        // QNetworkConfigurationManager can take a while, up to 5 seconds on Symbian.
        QNetworkConfigurationManager *network = new QNetworkConfigurationManager();
        emit done(network);
    }
};

/* NetworkUtils */

NetworkUtils::NetworkUtils(QObject *parent) :
    QObject(parent),
    network_(NULL),
    session_(NULL)
{
    DGUARDMETHODTIMED;
    PERFDATA_STARTDETAIL(cStartUp, "QNetworkConfigurationManager", "runtime");

#ifndef Q_OS_SYMBIAN
    initializeNetwork();
#else
    NetworkConfigurationRunnable *runnable = new NetworkConfigurationRunnable();
    connect(runnable, SIGNAL(done(QNetworkConfigurationManager*)), this, SLOT(networkConfigurationManagerDone(QNetworkConfigurationManager*)));
    QThreadPool::globalInstance()->start(runnable);
#endif

    PERFDATA_STOPDETAIL(cStartUp, "QNetworkConfigurationManager", "runtime");
}

void NetworkUtils::initializeNetwork()
{
    DGUARDMETHODTIMED;
    network_ = new QNetworkConfigurationManager(this);
    connectToNetworkConfigurationManager();
}

void NetworkUtils::networkConfigurationManagerDone(QNetworkConfigurationManager *result)
{
    DGUARDMETHODTIMED;
    network_ = result;
    connectToNetworkConfigurationManager();
}

void NetworkUtils::dumpNetworkConfigurations()
{
    DGUARDMETHODTIMED;
    if (network_)
    {
        DPRINTF("Count of all network configurations: %d", network_->allConfigurations().count());
        DPRINTF("Count of active network configurations: %d", network_->allConfigurations(QNetworkConfiguration::Active).count());
    }
}

void NetworkUtils::connectToNetworkConfigurationManager()
{
    DGUARDMETHODTIMED;
    if (network_)
    {
        DOP(dumpNetworkConfigurations());

        connect(network_, SIGNAL(onlineStateChanged(bool)), this, SIGNAL(onlineStateChanged()));
        connect(network_, SIGNAL(updateCompleted()), this, SLOT(handleNetworkUpdateCompleted()));

        QTimer::singleShot(2000, this, SLOT(deferredUpdateNetworkConfigurations()));
    }
}

void NetworkUtils::deferredUpdateNetworkConfigurations()
{
    DGUARDMETHODTIMED;
    if (network_)
    {
        network_->updateConfigurations();
        DOP(dumpNetworkConfigurations());
    }
}

void NetworkUtils::handleNetworkUpdateCompleted()
{
    DGUARDMETHODTIMED;
    DOP(dumpNetworkConfigurations());
    newNetworkSession();
    emit onlineStateChanged();
}

void NetworkUtils::newNetworkSession()
{
    DGUARDMETHODTIMED;
    if (!network_)
        return;

    if (session_)
        delete session_;

    session_ = new QNetworkSession(network_->defaultConfiguration(), this);
}

bool NetworkUtils::isOnline()
{
    return (network_ && network_->isOnline()) || (session_ && session_->isOpen());
}

bool NetworkUtils::connectedToCell()
{
    if (!network_)
        return false;

    const QNetworkConfiguration config = network_->defaultConfiguration();

    return config.bearerType() != QNetworkConfiguration::BearerEthernet &&
        config.bearerType() != QNetworkConfiguration::BearerWLAN &&
        config.bearerType() != QNetworkConfiguration::BearerUnknown;
}

bool NetworkUtils::attemptToOpenNetworkSession(int msecs)
{
    DGUARDMETHODTIMED;
    if (!session_)
        newNetworkSession();

    if (!session_)
        return false;

    if (!session_->isOpen())
    {
        session_->open();
        if (!session_->waitForOpened(msecs))
            return false;
    }

    return true;
}

#include "networkutils.moc"
