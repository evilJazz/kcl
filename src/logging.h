#ifndef LOGGING_H
#define LOGGING_H

#include <QObject>

class Logging
{
public:
    static Logging& singleton();
#	define gLogging Logging::singleton()

    void registerHandler();
    void unregisterHandler();

    void enableLogWindow();
    void disableLogWindow();

    void enableLogFile(const QString &fileName);
    void disableLogFile();

signals:

public slots:

protected:
    explicit Logging();

    static void customMessageHandler(QtMsgType type, const char *msg);
};

#endif // LOGGING_H
