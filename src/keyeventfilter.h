#ifndef KEYEVENTFILTER_H
#define KEYEVENTFILTER_H

#include <QObject>
#include <QKeyEvent>

class QDeclarativeKeyEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int key READ key)
    Q_PROPERTY(QString text READ text)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool isAutoRepeat READ isAutoRepeat)
    Q_PROPERTY(int count READ count)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)

public:
    QDeclarativeKeyEvent(QEvent::Type type, int key, Qt::KeyboardModifiers modifiers, const QString &text=QString(), bool autorep=false, ushort count=1)
        : event(type, key, modifiers, text, autorep, count) { event.setAccepted(false); }
    QDeclarativeKeyEvent(const QKeyEvent &ke)
        : event(ke) { event.setAccepted(false); }

    int key() const { return event.key(); }
    QString text() const { return event.text(); }
    int modifiers() const { return event.modifiers(); }
    bool isAutoRepeat() const { return event.isAutoRepeat(); }
    int count() const { return event.count(); }

    bool isAccepted() { return event.isAccepted(); }
    void setAccepted(bool accepted) { event.setAccepted(accepted); }

private:
    QKeyEvent event;
};

class KeyEventFilter : public QObject
{
    Q_OBJECT
public:
    explicit KeyEventFilter(QObject *parent = 0);
    virtual ~KeyEventFilter();
    
signals:
    void keyPressed(QDeclarativeKeyEvent *event);
    void keyReleased(QDeclarativeKeyEvent *event);

protected:
    virtual bool eventFilter(QObject *watched, QEvent *event);
};

#endif // KEYEVENTFILTER_H
