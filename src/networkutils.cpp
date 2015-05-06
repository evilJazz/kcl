#include "KCL/networkutils.h"
#include "KCL/performancedatamanager.h"

#include <QThreadPool>
#include <QTimer>

#define DIAGNOSTIC_OUTPUT
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
    network_ = new QNetworkConfigurationManager(this);
    connectToNetworkConfigurationManager();
#else
    NetworkConfigurationRunnable *runnable = new NetworkConfigurationRunnable();
    connect(runnable, SIGNAL(done(QNetworkConfigurationManager*)), this, SLOT(networkConfigurationManagerDone(QNetworkConfigurationManager*)));
    QThreadPool::globalInstance()->start(runnable);
#endif

    PERFDATA_STOPDETAIL(cStartUp, "QNetworkConfigurationManager", "runtime");
}

void NetworkUtils::deferredInitializeNetwork()
{
    DGUARDMETHODTIMED;
    network_ = new QNetworkConfigurationManager(this);
    connectToNetworkConfigurationManager();
    QTimer::singleShot(2000, this, SLOT(deferredUpdateNetworkConfigurations()));
}

void NetworkUtils::networkConfigurationManagerDone(QNetworkConfigurationManager *result)
{
    DGUARDMETHODTIMED;
    network_ = result;
    connectToNetworkConfigurationManager();
    QTimer::singleShot(2000, this, SLOT(deferredUpdateNetworkConfigurations()));
}

void NetworkUtils::connectToNetworkConfigurationManager()
{
    if (network_)
    {
        connect(network_, SIGNAL(onlineStateChanged(bool)), this, SIGNAL(onlineStateChanged()));
        connect(network_, SIGNAL(updateCompleted()), this, SLOT(handleNetworkUpdateCompleted()));
    }
}

void NetworkUtils::deferredUpdateNetworkConfigurations()
{
    DGUARDMETHODTIMED;
    if (network_)
        network_->updateConfigurations();
}

void NetworkUtils::handleNetworkUpdateCompleted()
{
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
