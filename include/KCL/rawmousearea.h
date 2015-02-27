#ifndef RAWMOUSEAREA_H
#define RAWMOUSEAREA_H

#include <QDeclarativeItem>
#include <QGraphicsSceneMouseEvent>
#include <QPointF>

class DeclarativeMouseEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int x READ x)
    Q_PROPERTY(int y READ y)
    Q_PROPERTY(int button READ button)
    Q_PROPERTY(int buttons READ buttons)
    Q_PROPERTY(int modifiers READ modifiers)
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted)
public:
    DeclarativeMouseEvent(int x, int y, Qt::MouseButton button, Qt::MouseButtons buttons, Qt::KeyboardModifiers modifiers)
        : x_(x), y_(y), button_(button), buttons_(buttons), modifiers_(modifiers), accepted_(true) {}

    int x() const { return x_; }
    int y() const { return y_; }
    int button() const { return button_; }
    int buttons() const { return buttons_; }
    int modifiers() const { return modifiers_; }

    bool isAccepted() { return accepted_; }
    void setAccepted(bool accepted) { accepted_ = accepted; }

private:
    int x_;
    int y_;
    Qt::MouseButton button_;
    Qt::MouseButtons buttons_;
    Qt::KeyboardModifiers modifiers_;
    bool accepted_;
};

class RawMouseArea : public QDeclarativeItem
{
    Q_OBJECT
public:
    RawMouseArea(QDeclarativeItem *parent = 0);
    virtual ~RawMouseArea();

    Q_INVOKABLE void grabMouse();
    Q_INVOKABLE void ungrabMouse();

signals:
    void pressed(DeclarativeMouseEvent *mouse);
    void moved(DeclarativeMouseEvent *mouse);
    void released(DeclarativeMouseEvent *mouse);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
};

#endif // RAWMOUSEAREA_H
