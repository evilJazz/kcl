#ifndef DECLARATIVEDEBUG_H
#define DECLARATIVEDEBUG_H

#include <QObject>

class QDeclarativeEngine;
class QScriptEngine;
class QScriptContextInfo;

class DeclarativeDebug : public QObject
{
    Q_OBJECT
public:
    explicit DeclarativeDebug(QDeclarativeEngine *engine);
    virtual ~DeclarativeDebug();

public slots:
    void enterMethod(const QString &text = QString::null);
    void exitMethod(const QString &text = QString::null);

    void print(const QString &text);

private:
    QDeclarativeEngine *engine_;
    QScriptEngine *scriptEngine_;

    static QString getFunctionSignature(QScriptContextInfo *info);
};

#endif // DECLARATIVEDEBUG_H
