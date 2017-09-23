#ifndef SYSTEMUTILS_H
#define SYSTEMUTILS_H

#include <QObject>

class SystemUtils : public QObject
{
    Q_OBJECT
public:
    explicit SystemUtils(QObject *parent = nullptr);

signals:

public slots:
};

#endif // SYSTEMUTILS_H