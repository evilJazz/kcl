#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QRunnable>

class NetworkUtils : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool isOnline READ isOnline NOTIFY onlineStateChanged)
    Q_PROPERTY(bool connectedToCell READ connectedToCell NOTIFY onlineStateChanged)
public:
    explicit NetworkUtils(QObject *parent = 0);

    bool isOnline();
    bool connectedToCell();

    Q_INVOKABLE bool attemptToOpenNetworkSession(int msecs = 30000);

signals:
    void onlineStateChanged();

private slots:
    void deferredInitializeNetwork();
    void networkConfigurationManagerDone(QNetworkConfigurationManager *result);
    void deferredUpdateNetworkConfigurations();
    void handleNetworkUpdateCompleted();

private:
    QNetworkConfigurationManager *network_;
    QNetworkSession *session_;

    void newNetworkSession();
    void connectToNetworkConfigurationManager();
};

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
#endif // NETWORKUTILS_H
