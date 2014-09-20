#ifndef DECLARATIVEDEBUG_H
#define DECLARATIVEDEBUG_H

#include "KCL/kcl_global.h"

#include <QObject>

#ifdef KCL_QTQUICK2
class QQmlEngine;
#else
class QDeclarativeEngine;
class QScriptEngine;
class QScriptContextInfo;
#endif

class KCL_EXPORT DeclarativeDebug : public QObject
{
    Q_OBJECT
public:
#ifdef KCL_QTQUICK2
    explicit DeclarativeDebug(QQmlEngine *engine);
#else
    explicit DeclarativeDebug(QDeclarativeEngine *engine);
#endif
    virtual ~DeclarativeDebug();

public slots:
    void enterMethod(const QString &text = QString::null);
    void exitMethod(const QString &text = QString::null);

    void print(const QString &text);

private:
#ifdef KCL_QTQUICK2
    QQmlEngine *engine_;
#else
    QDeclarativeEngine *engine_;
    QScriptEngine *scriptEngine_;

    static QString getFunctionSignature(QScriptContextInfo *info);
#endif
};

#endif // DECLARATIVEDEBUG_H
