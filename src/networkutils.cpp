#include "KCL/networkutils.h"
#include "KCL/performancedatamanager.h"

#include <QThreadPool>
#include <QTimer>

KCL_USE_NAMESPACE

/* NetworkUtils */

NetworkUtils::NetworkUtils(QObject *parent) :
    QObject(parent),
    network_(NULL),
    session_(NULL)
{
    PERFDATA_STARTDETAIL(cStartUp, "QNetworkConfigurationManager", "runtime");

    //network_ = new QNetworkConfigurationManager(this);
    //connectToNetworkConfigurationManager();

    NetworkConfigurationRunnable *runnable = new NetworkConfigurationRunnable();
    connect(runnable, SIGNAL(done(QNetworkConfigurationManager*)), this, SLOT(networkConfigurationManagerDone(QNetworkConfigurationManager*)));
    QThreadPool::globalInstance()->start(runnable);

    //QTimer::singleShot(6000, this, SLOT(deferredInitializeNetwork()));

    PERFDATA_STOPDETAIL(cStartUp, "QNetworkConfigurationManager", "runtime");
}

void NetworkUtils::deferredInitializeNetwork()
{
    network_ = new QNetworkConfigurationManager(this);
    connectToNetworkConfigurationManager();
    QTimer::singleShot(2000, this, SLOT(deferredUpdateNetworkConfigurations()));
}

void NetworkUtils::networkConfigurationManagerDone(QNetworkConfigurationManager *result)
{
    network_ = result;
    connectToNetworkConfigurationManager();
    QTimer::singleShot(2000, this, SLOT(deferredUpdateNetworkConfigurations()));
}

void NetworkUtils::connectToNetworkConfigurationManager()
{
    connect(network_, SIGNAL(onlineStateChanged(bool)), this, SIGNAL(onlineStateChanged()));
    connect(network_, SIGNAL(updateCompleted()), this, SLOT(handleNetworkUpdateCompleted()));
}

void NetworkUtils::deferredUpdateNetworkConfigurations()
{
    network_->updateConfigurations();
}

void NetworkUtils::handleNetworkUpdateCompleted()
{
    newNetworkSession();
    emit onlineStateChanged();
}

void NetworkUtils::newNetworkSession()
{
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
    if (!session_)
        newNetworkSession();

    if (!session_->isOpen())
    {
        session_->open();
        if (!session_->waitForOpened(msecs))
            return false;
    }

    return true;
}

