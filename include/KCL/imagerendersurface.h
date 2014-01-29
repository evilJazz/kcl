#ifndef IMAGERENDERSURFACE_H
#define IMAGERENDERSURFACE_H

#include "KCL/kcl_global.h"

#include <QImage>

#ifdef KCL_QTQUICK2
    #include <QQuickPaintedItem>
#else
    #include <QDeclarativeItem>
#endif

namespace KCL {

class Task;

class KCL_EXPORT ImageRenderSurface :
#ifdef KCL_QTQUICK2
    public QQuickPaintedItem
#else
    public QDeclarativeItem
#endif
{
    Q_OBJECT
    Q_ENUMS(Status)
    Q_PROPERTY(QUrl source READ source WRITE setSource NOTIFY sourceChanged)
    Q_PROPERTY(Status status READ status NOTIFY statusChanged)
public:
    enum Status
    {
        Null = 0,
        Rendering = 1,
        Rendered = 2
    };

#ifdef KCL_QTQUICK2
    explicit ImageRenderSurface(QQuickItem *parent = 0);
#else
    explicit ImageRenderSurface(QDeclarativeItem *parent = 0);
#endif
    virtual ~ImageRenderSurface();

    Q_INVOKABLE void setSource(QUrl source);
    Q_INVOKABLE QUrl source() const { return source_; }

    Q_INVOKABLE Status status() const { return status_; }

protected:
#ifdef KCL_QTQUICK2
    virtual void paint(QPainter *);
#else
    virtual void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);
#endif
    virtual void geometryChanged(const QRectF & newGeometry, const QRectF & oldGeometry);

signals:
    void sourceChanged();
    void statusChanged();

private slots:
    void updateSurface();

private:
    friend class RenderTask;
    QUrl source_;
    QImage surface_;
    Task *task_;
    Status status_;

    void removeTask();
    void setStatus(Status newStatus);
};

}

#endif // IMAGERENDERSURFACE_H
