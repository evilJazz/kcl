/****************************************************************************
**
** Copyright (C) 2011-2016 Andre Beckedorf
** Contact: <evilJazz _AT_ katastrophos _DOT_ net>
**
** This file is part of the Katastrophos.net Component Library (KCL)
**
** $KCL_BEGIN_LICENSE$
** GNU Lesser General Public License Usage
** This library is free software; you can redistribute it and/or modify
** it under the terms of the GNU Lesser General Public License version
** 2.1 or 3.0 as published by the Free Software Foundation.
**
** This library is distributed in the hope that it will be useful, but
** WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
** Lesser General Public License for more details:
** https://www.gnu.org/licenses/lgpl-2.1.html
** https://www.gnu.org/licenses/lgpl-3.0.html
**
** You should have received a copy of the GNU Lesser General Public
** License along with this library; if not, write to the Free Software
** Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
** 02110-1301  USA
**
** Mozilla Public License Usage
** Alternatively, this file is available under the Mozilla Public
** License Version 1.1.  You may obtain a copy of the License at
** http://www.mozilla.org/MPL/
**
** $KCL_END_LICENSE$
**
****************************************************************************/

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

#ifndef KCL_DEBUG
#undef DEBUG
#endif
#include "KCL/debug.h"

Q_GLOBAL_STATIC(TaskProcessingController, tpc)

class RenderTask : public Task
{
public:
    void execute()
    {
        QSize realSize;
        QImage image = provider->requestImage(imageId, &realSize, requestedSize);

        QSize newSize = image.size();
        newSize.scale(requestedSize, Qt::KeepAspectRatio);

        QImage dstImage(newSize, QImage::Format_ARGB32_Premultiplied);      
        dstImage.fill(0);

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

    tpc()->addTask(task_);
}

void ImageRenderSurface::removeTask()
{
    if (task_)
    {
        static_cast<RenderTask *>(task_)->surface = NULL;

        if (tpc()->isTaskEnqueued(task_))
            tpc()->removeTask(task_);

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
