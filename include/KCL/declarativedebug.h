#ifndef DECLARATIVEDEBUG_H
#define DECLARATIVEDEBUG_H

#include "KCL/kcl_global.h"

#include <QObject>

class QDeclarativeEngine;
class QScriptEngine;
class QScriptContextInfo;

namespace KCL {

class KCL_EXPORT DeclarativeDebug : public QObject
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

}

#endif // DECLARATIVEDEBUG_H
