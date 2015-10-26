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
#ifdef KCL_NETWORKUTILS_FORCE_ALWAYS_ONLINE
    return true;
#else
    return (network_ && network_->isOnline()) || (session_ && session_->isOpen());
#endif
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
