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

#ifndef IMAGERENDERSURFACE_H
#define IMAGERENDERSURFACE_H

#include "KCL/kcl_global.h"

#include <QImage>

#ifdef KCL_QTQUICK2
    #include <QQuickPaintedItem>
#else
    #include <QDeclarativeItem>
#endif

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

#endif // IMAGERENDERSURFACE_H
