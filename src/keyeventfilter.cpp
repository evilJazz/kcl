#include "keyeventfilter.h"

KeyEventFilter::KeyEventFilter(QObject *parent) :
    QObject(parent)
{
}

KeyEventFilter::~KeyEventFilter()
{
}

bool KeyEventFilter::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() == QEvent::KeyPress)
    {
        QDeclarativeKeyEvent qke(*static_cast<QKeyEvent *>(event));
        emit keyPressed(&qke);
        if (qke.isAccepted())
            return true;
    }
    else if (event->type() == QEvent::KeyRelease)
    {
        QDeclarativeKeyEvent qke(*static_cast<QKeyEvent *>(event));
        emit keyReleased(&qke);
        if (qke.isAccepted())
            return true;
    }

    return QObject::eventFilter(watched, event);
}
