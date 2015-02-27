#include "KCL/rawmousearea.h"

#include <QGraphicsScene>

RawMouseArea::RawMouseArea(QDeclarativeItem *parent) :
    QDeclarativeItem(parent)
{
}

RawMouseArea::~RawMouseArea()
{
}

void RawMouseArea::grabMouse()
{
    QDeclarativeItem::grabMouse();
}

void RawMouseArea::ungrabMouse()
{
    QDeclarativeItem::ungrabMouse();
}

void RawMouseArea::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = mapFromScene(event->scenePos());
    DeclarativeMouseEvent ev(pos.x(), pos.y(), event->button(), event->buttons(), event->modifiers());

    emit moved(&ev);

    if (ev.isAccepted())
        event->accept();
    else
        event->ignore();
}

void RawMouseArea::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = mapFromScene(event->scenePos());
    DeclarativeMouseEvent ev(pos.x(), pos.y(), event->button(), event->buttons(), event->modifiers());

    emit pressed(&ev);

    if (ev.isAccepted())
        event->accept();
    else
        event->ignore();
}

void RawMouseArea::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    QPointF pos = mapFromScene(event->scenePos());
    DeclarativeMouseEvent ev(pos.x(), pos.y(), event->button(), event->buttons(), event->modifiers());

    emit released(&ev);

    if (ev.isAccepted())
        event->accept();
    else
        event->ignore();
}
