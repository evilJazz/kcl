#ifndef KCLPLUGIN_H
#define KCLPLUGIN_H

#include <QDeclarativeExtensionPlugin>

class KCLPlugin : public QDeclarativeExtensionPlugin
{
    Q_OBJECT
public:
    void registerTypes(const char *uri);
    void initializeEngine(QDeclarativeEngine *engine, const char *uri);
};

#endif // KCLPLUGIN_H
