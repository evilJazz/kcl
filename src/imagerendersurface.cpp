#include "KCL/debug.h"
#include "KCL/backgroundtasks.h"
#include "KCL/imagerendersurface.h"

#include <QPainter>
#include <QSize>
#include <QSizeF>

#ifdef KCL_QTQUICK2
    #include <QQmlEngine>
    #include <QQmlContext>
    #include <QQuickImageProvider>
    #define QDeclarativeItem QQuickItem
    #define QDeclarativePaintedItem QQuickPaintedItem
    #define QDeclarativeImageProvider QQuickImageProvider
    #define QDeclarativeContext QQmlContext
    #define QDeclarativeEngine QQmlEngine
#else
    #include <QDeclarativeEngine>
    #include <QDeclarativeContext>
    #include <QDeclarativeImageProvider>
    #define QDeclarativePaintedItem QDeclarativeItem
#endif

static TaskProcessingController *instance = NULL;

TaskProcessingController &tpc()
{
    if (!instance)
        instance = new TaskProcessingController();

    return *instance;
}

QSize optimalFitSize(const QSize &srcSize, const QSize &maxDstSize)
{
    qreal cx, cy;
    qreal rscaleX, rscaleY;

    cx = srcSize.width();
    cy = srcSize.height();

    if (!cx || !cy)
        return QSize(0, 0);

    rscaleX = maxDstSize.width() / cx;
    rscaleY = maxDstSize.height() / cy;

    if (rscaleX >= rscaleY)
    {
        cx = cx * rscaleY;
        cy = maxDstSize.height();
    }
    else
    {
        cx = maxDstSize.width();
        cy = cy * rscaleX;
    }

    return QSize(cx, cy);
}


class RenderTask : public Task
{
public:
    void execute()
    {
        QSize realSize;
        QImage image = provider->requestImage(imageId, &realSize, requestedSize);

        QSize newSize = optimalFitSize(image.size(), requestedSize);

        QImage dstImage(newSize, QImage::Format_ARGB32_Premultiplied);

        QPainter painter(&dstImage);
        painter.drawImage(dstImage.rect(), image);

        result = dstImage;
    }

    void postExecute()
    {
        if (surface)
        {
            surface->surface_ = result;
            surface->task_ = NULL;
            surface->setStatus(ImageRenderSurface::Rendered);
            surface->update();
        }
    }

    ImageRenderSurface *surface;
    QDeclarativeImageProvider *provider;
    QString imageId;
    QSize requestedSize;
    QImage result;
};


ImageRenderSurface::ImageRenderSurface(QDeclarativeItem *parent) :
    QDeclarativePaintedItem(parent),
    task_(NULL),
    status_(Null)
{
#ifndef KCL_QTQUICK2
    setFlag(QGraphicsItem::ItemHasNoContents, false);
#endif
}

ImageRenderSurface::~ImageRenderSurface()
{
    removeTask();
}

void ImageRenderSurface::setSource(QUrl source)
{
    if (source != source_)
    {
        source_ = source;

        if (source_.isEmpty())
        {
            removeTask();
            surface_ = QImage();
            setStatus(ImageRenderSurface::Null);
        }
        else
            updateSurface();

        emit sourceChanged();
    }
}

#ifdef KCL_QTQUICK2
void ImageRenderSurface::paint(QPainter *painter)
#else
void ImageRenderSurface::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
#endif
{
    if (surface_.isNull())
        return;

    QRect dstRect = surface_.rect();
    dstRect.moveCenter(boundingRect().center().toPoint());
    painter->drawImage(dstRect.topLeft(), surface_);
}

void ImageRenderSurface::geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry)
{
    if (newGeometry.size() != oldGeometry.size())
        updateSurface();

    QDeclarativeItem::geometryChanged(newGeometry, oldGeometry);
}

void ImageRenderSurface::updateSurface()
{
    DGUARDMETHODTIMED;

    if (source_.isEmpty())
        return;

    QSize requestedSize = QSize(width(), height());
    if (requestedSize.isEmpty() || !requestedSize.isValid())
    {
        DPRINTF("requestedSize not suitable: %d x %d", requestedSize.width(), requestedSize.height());
        return;
    }

    QDeclarativeContext *context = QDeclarativeEngine::contextForObject(this);
    if (!context)
        return;

    QDeclarativeEngine *engine = context->engine();

    QString providerId = source_.host();
    QString imageId = source_.path().mid(1) + source_.fragment();

    QDeclarativeImageProvider *provider = dynamic_cast<QDeclarativeImageProvider *>(engine->imageProvider(providerId));
    if (!provider)
        return;

    if (provider->imageType() != QDeclarativeImageProvider::Image)
        return;

    setStatus(ImageRenderSurface::Rendering);

    removeTask();

    RenderTask *rt = new RenderTask();
    rt->requestedSize = requestedSize;
    rt->surface = this;
    rt->imageId = imageId;
    rt->provider = provider;

    task_ = rt;

    tpc().addTask(task_);
}

void ImageRenderSurface::removeTask()
{
    if (task_)
    {
        static_cast<RenderTask *>(task_)->surface = NULL;

        if (tpc().isTaskEnqueued(task_))
            tpc().removeTask(task_);

        task_ = NULL;
    }
}

void ImageRenderSurface::setStatus(ImageRenderSurface::Status newStatus)
{
    if (newStatus != status_)
    {
        status_ = newStatus;
        emit statusChanged();
    }
}
