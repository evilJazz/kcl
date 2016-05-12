#include "../include/KCL/touchdetector.h"

#ifdef KCL_QTQUICK2
#include <QCursor>
#define QDeclarativeItem QQuickItem
#endif

TouchDetector::TouchDetector(QDeclarativeItem *parent) :
    QDeclarativeItem(parent),
    touchDeviceActive_(false)
{
}

TouchDetector::~TouchDetector()
{
}

bool TouchDetector::touchDeviceActive() const
{
    return touchDeviceActive_;
}

#ifdef KCL_QTQUICK2
void TouchDetector::touchEvent(QTouchEvent *event)
{
    setTouchDeviceActive(true);
    event->ignore();
    QDeclarativeItem::touchEvent(event);
}

void TouchDetector::mousePressEvent(QMouseEvent *event)
{
    setTouchDeviceActive(false);
    event->ignore();
    QDeclarativeItem::mousePressEvent(event);
}

void TouchDetector::mouseMoveEvent(QMouseEvent *event)
{
    setTouchDeviceActive(false);
    event->ignore();
    QDeclarativeItem::mouseMoveEvent(event);
}

void TouchDetector::mouseReleaseEvent(QMouseEvent *event)
{
    setTouchDeviceActive(false);
    event->ignore();
    QDeclarativeItem::mouseReleaseEvent(event);
}

void TouchDetector::wheelEvent(QWheelEvent *event)
{
    setTouchDeviceActive(false);
    event->ignore();
    QDeclarativeItem::wheelEvent(event);
}

#endif

void TouchDetector::setTouchDeviceActive(bool value)
{
    if (touchDeviceActive_ != value)
    {
        touchDeviceActive_ = value;
        emit touchDeviceActiveChanged();
    }
}
