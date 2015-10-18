#ifndef NETWORKUTILS_H
#define NETWORKUTILS_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QNetworkConfigurationManager>
#include <QNetworkSession>
#include <QRunnable>

class KCL_EXPORT NetworkUtils : public QObject
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
    void initializeNetwork();
    void networkConfigurationManagerDone(QNetworkConfigurationManager *result);
    void deferredUpdateNetworkConfigurations();
    void handleNetworkUpdateCompleted();

private:
    QNetworkConfigurationManager *network_;
    QNetworkSession *session_;

    void newNetworkSession();
    void connectToNetworkConfigurationManager();

    void dumpNetworkConfigurations();
};

#endif // NETWORKUTILS_H
