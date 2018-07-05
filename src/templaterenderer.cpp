/****************************************************************************
**
** Copyright (C) 2018 Andre Beckedorf
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

#include "KCL/templaterenderer.h"

#include "KCL/filesystemutils.h"

//#define DIAGNOSTIC_OUTPUT
#include "KCL/debug.h"

TemplateRenderer::TemplateRenderer(QObject *parent) :
    PropertyChangeObserver(parent),
    parentRenderer_(NULL),
    name_(),
    childPrefix_(),
    templateText_(),
    templateSource_(),
    templateRegEx_("\\$\\{(.*)\\}"),
    content_(),
    contentDirty_(true),
    renderDelay_(0),
    renderTimer_(NULL),
    topLevelRenderer_(this)
{
    setRenderDelay(10);
    connect(this, SIGNAL(propertyChanged(QString)), this, SLOT(markContentDirty()));
    connect(this, SIGNAL(subRendererTemplateChanged(TemplateRenderer*)), this, SLOT(notifyParentRenderer()));

    ignoredPropertyNames_ << "parentRenderer";
    ignoredPropertyNames_ << "topLevelRenderer";
    ignoredPropertyNames_ << "templateText";
    ignoredPropertyNames_ << "templateSource";
    ignoredPropertyNames_ << "templateRegEx";
    ignoredPropertyNames_ << "markersUsedInTemplate";
    ignoredPropertyNames_ << "content";
    ignoredPropertyNames_ << "contentDirty";
    ignoredPropertyNames_ << "renderDelay";
    ignoredPropertyNames_ << "children";

    setIgnoredPropertyNames(ignoredPropertyNames_);

    connect(this, SIGNAL(parentChanged()), this, SLOT(handleParentChanged()));
    handleParentChanged();
}

TemplateRenderer::~TemplateRenderer()
{
    setParentRenderer(NULL);
}

void TemplateRenderer::removeRenderer(TemplateRenderer *childRenderer)
{
    subRenderersMap_.remove(childRenderer->name());
    subRenderers_.removeAll(childRenderer);
    emit subRenderersChanged();
}

void TemplateRenderer::addRenderer(TemplateRenderer *childRenderer)
{
    subRenderersMap_.insert(childRenderer->name(), QPointer<TemplateRenderer>(childRenderer));
    subRenderers_.prepend(childRenderer);
    emit subRenderersChanged();
}

void TemplateRenderer::setParentRenderer(TemplateRenderer *newParentRenderer)
{
    if (parentRenderer_)
    {
        disconnect(parentRenderer_, SIGNAL(topLevelRendererChanged()), this, SLOT(handleParentRendererTopLevelRendererChanged()));
        parentRenderer_->removeRenderer(this);
        parentRenderer_.clear();
    }

    parentRenderer_ = newParentRenderer;

    if (parentRenderer_)
    {
        parentRenderer_->addRenderer(this);
        connect(parentRenderer_, SIGNAL(topLevelRendererChanged()), this, SLOT(handleParentRendererTopLevelRendererChanged()));
    }

    TemplateRenderer *newTopLevel = this;
    TemplateRenderer *renderer = parentRenderer_;

    while (renderer)
    {
        newTopLevel = renderer;
        renderer = renderer->parentRenderer();
    }

    setTopLevelRenderer(newTopLevel);

    emit parentRendererChanged();
}

TemplateRenderer *TemplateRenderer::parentRenderer() const
{
    return parentRenderer_;
}

TemplateRenderer *TemplateRenderer::topLevelRenderer() const
{
    return topLevelRenderer_;
}

QString TemplateRenderer::name() const
{
    return name_;
}

void TemplateRenderer::setName(const QString &name)
{
    if (name != name_)
    {
        name_ = name;
        emit nameChanged();
    }
}

QString TemplateRenderer::identifier() const
{
    QString result;

    if (parentRenderer_ && parentRenderer_->childPrefix().length() > 0)
    {
        result = parentRenderer_->childPrefix() + "_";
    }

    result += name_;
    return result;
}

QString TemplateRenderer::childPrefix() const
{
    if (childPrefix_.length() > 0)
        return childPrefix_;
    else
        return parentRenderer_ ? parentRenderer_->childPrefix() : "";
}

void TemplateRenderer::setChildPrefix(const QString &childPrefix)
{
    if (childPrefix != childPrefix_)
    {
        childPrefix_ = childPrefix;
        emit childPrefixChanged();
    }
}

QString TemplateRenderer::templateText() const
{
    return templateText_;
}

void TemplateRenderer::setTemplateText(const QString &templateText)
{
    if (templateText != templateText_)
    {
        templateText_ = templateText;
        handleTemplateChanged();
        emit templateTextChanged();
    }
}

QUrl TemplateRenderer::templateSource() const
{
    return templateSource_;
}

void TemplateRenderer::setTemplateSource(const QUrl &url)
{
    if (url != templateSource_)
    {
        templateSource_ = url;
        templateText_.clear();
        handleTemplateChanged();
        emit templateSourceChanged();
    }
}

QString TemplateRenderer::templateRegEx() const
{
    return templateRegEx_;
}

void TemplateRenderer::setTemplateRegEx(const QString &templateRegEx)
{
    if (templateRegEx != templateRegEx_)
    {
        templateRegEx_ = templateRegEx;
        handleTemplateChanged();
        emit templateRegExChanged();
    }
}

QStringList TemplateRenderer::markersUsedInTemplate() const
{
    return markersUsedInTemplate_.keys();
}

QString TemplateRenderer::content() const
{
    return content_;
}

bool TemplateRenderer::contentDirty() const
{
    return contentDirty_;
}

int TemplateRenderer::renderDelay() const
{
    return renderDelay_;
}

void TemplateRenderer::setRenderDelay(int delay)
{
    if (delay != renderDelay_)
    {
        renderDelay_ = delay;
        emit renderDelayChanged();

        if (renderDelay_ > 0 && !renderTimer_)
        {
            renderTimer_ = new QTimer(this);
            renderTimer_->setSingleShot(true);
            connect(renderTimer_, SIGNAL(timeout()), this, SLOT(updateContent()));
        }
        else if (renderTimer_)
        {
            renderTimer_->deleteLater();
            renderTimer_.clear();
        }
    }
}

void TemplateRenderer::updateContent()
{
    DGUARDMETHODTIMED;

    if (templateText_.isEmpty() && !templateSource_.isEmpty())
        templateText_ = FileSystemUtils::getContents(FileSystemUtils::localPathFromUrl(templateSource_));

    QString newContent;

    QRegExp rx(templateRegEx_);
    rx.setMinimal(true);
    int startPos = 0;
    int currentPos = 0;

    while ((currentPos = rx.indexIn(templateText_, startPos)) != -1)
    {
        newContent += templateText_.mid(startPos, currentPos - startPos);
        startPos = currentPos + rx.matchedLength();

        QString name = rx.cap(1);
        QString replacement;
        QVariant replacementVar;

        if (this->hasOwnProperty(name))
        {
            if (!QMetaObject::invokeMethod(
                    this,
                    "replaceMarkerForProperty",
                    Q_RETURN_ARG(QVariant, replacementVar),
                    Q_ARG(QVariant, name)
                ))
            {
                replacement = _TemplateRenderer_replaceMarkerForProperty(name);
            }
            else
            {
                replacement = replacementVar.toString();
            }
        }
        else
        {
            TemplateRenderer *subRenderer = findTemplateRendererByName(name);
            if (subRenderer)
            {
                if (subRenderer->contentDirty())
                    subRenderer->updateContent();

                if (!QMetaObject::invokeMethod(
                        subRenderer,
                        "replaceMarkerForContent",
                        Q_RETURN_ARG(QVariant, replacementVar)
                    ))
                {
                    replacement = _TemplateRenderer_replaceMarkerForContent();
                }
                else
                {
                    replacement = replacementVar.toString();
                }
            }
        }

        newContent += replacement;
    }

    if (currentPos == -1)
        newContent += templateText_.mid(startPos, templateText_.length() - startPos);

    content_ = newContent;
    contentDirty_ = false;

    emit contentDirtyChanged();
    emit contentChanged();
}

TemplateRenderer *TemplateRenderer::findTemplateRendererByName(const QString &name)
{
    return subRenderersMap_.value(name).data();
}

bool TemplateRenderer::isTemplateRenderer(QObject *item)
{
    if (item)
    {
        DPRINTF("class name: %s, objectName: %s, isTemplateRenderer: %s",
            item->metaObject()->className(),
            item->objectName().toUtf8().constData(),
            qobject_cast<TemplateRenderer *>(item) ? "true" : "false"
        );
    }
    return item && qobject_cast<TemplateRenderer *>(item) != NULL;
}

void TemplateRenderer::dumpRendererStructure()
{
    kaPrintf("%s name: %s, childPrefix: %s, identifier: %s, contentDirty: %s",
        metaObject()->className(),
        name().toUtf8().constData(),
        childPrefix().toUtf8().constData(),
        identifier().toUtf8().constData(),
        contentDirty() ? "true" : "false"
    );

    dumpRendererStructure("    ");
}

void TemplateRenderer::dumpRendererStructure(const QString &indent)
{
    int itemNr = 0;

    foreach (TemplateRenderer *renderer, subRenderers_)
    {
        if (renderer)
        {
            kaPrintf("%s %d: %s name: %s, childPrefix: %s, identifier: %s, contentDirty: %s, topLevelRenderer: %s (%s)",
                indent.toUtf8().constData(),
                itemNr,
                renderer->metaObject()->className(),
                renderer->name().toUtf8().constData(),
                renderer->childPrefix().toUtf8().constData(),
                renderer->identifier().toUtf8().constData(),
                renderer->contentDirty() ? "true" : "false",
                renderer->topLevelRenderer()->metaObject()->className(),
                renderer->topLevelRenderer()->identifier().toUtf8().constData()
            );

            renderer->dumpRendererStructure(indent + "    ");
            ++itemNr;
        }
    }
}

QStringList TemplateRenderer::_TemplateRenderer_ignoredPropertyNames() const
{
    return ignoredPropertyNames_;
}

QString TemplateRenderer::_TemplateRenderer_replaceMarkerForProperty(const QString &propertyName)
{
    return this->getProperty(propertyName).toString();
}

QString TemplateRenderer::_TemplateRenderer_replaceMarkerForContent()
{
    return this->content_;
}

QString TemplateRenderer::replaceMarkerForProperty(const QVariant &propertyName)
{
    return _TemplateRenderer_replaceMarkerForProperty(propertyName.toString());
}

QString TemplateRenderer::replaceMarkerForContent()
{
    return _TemplateRenderer_replaceMarkerForContent();
}

bool TemplateRenderer::isMarkerUsedInTemplate(const QString &name) const
{
    return markersUsedInTemplate_.contains(name);
}

void TemplateRenderer::handleTemplateChanged()
{
    if (templateText_.isEmpty() == 0 && !templateSource_.isEmpty())
        setTemplateText(FileSystemUtils::getContents(FileSystemUtils::localPathFromUrl(templateSource_)));

    QMap<QString, int> makersUsed;

    QRegExp rx(templateRegEx_);
    rx.setMinimal(true);
    int startPos = 0;
    int currentPos = 0;
    DPRINTF("rx.errorString(): %s", rx.errorString().toUtf8().constData());

    while ((currentPos = rx.indexIn(templateText_, startPos)) != -1)
    {
        startPos = currentPos + rx.matchedLength();
        QString name = rx.cap(1);
        ++makersUsed[name];
    }

    markersUsedInTemplate_ = makersUsed;
    emit markersUsedInTemplateChanged();

    notifyParentRenderer();
}

void TemplateRenderer::handleParentChanged()
{
    if (isTemplateRenderer(parent()))
        setParentRenderer(qobject_cast<TemplateRenderer *>(parent()));
}

void TemplateRenderer::notifyParentRenderer()
{
    markContentDirty();

    if (parentRenderer_)
        emit parentRenderer_->subRendererTemplateChanged(this);
}

void TemplateRenderer::markContentDirty()
{
    if (!contentDirty_)
    {
        contentDirty_ = true;
        emit contentDirtyChanged();
    }

    if (renderDelay_ == 0)
        updateContent();
    else if (renderDelay_ > 0 && renderTimer_)
        renderTimer_->start(renderDelay_);

    if (parentRenderer_)
        parentRenderer_->markContentDirty();
}

void TemplateRenderer::handleParentRendererTopLevelRendererChanged()
{
    if (parentRenderer_)
        setTopLevelRenderer(parentRenderer_->topLevelRenderer());
    else
        setTopLevelRenderer(this);
}

void TemplateRenderer::setTopLevelRenderer(TemplateRenderer *renderer)
{
    if (renderer != topLevelRenderer_)
    {
        topLevelRenderer_ = renderer;
        emit topLevelRendererChanged();
    }
}

DeclarativeListProperty<TemplateRenderer> TemplateRenderer::subRenderers()
{
    return DeclarativeListProperty<TemplateRenderer>(this, subRenderers_);
}

DeclarativeListProperty<QObject> TemplateRenderer::children()
{
    return DeclarativeListProperty<QObject>(this, this,
        &TemplateRenderer::appendChild,
        &TemplateRenderer::childrenCount,
        &TemplateRenderer::child,
        &TemplateRenderer::clearChildren
    );
}

void TemplateRenderer::appendChild(DeclarativeListProperty<QObject> *list, QObject *child)
{
    TemplateRenderer *renderer = static_cast<TemplateRenderer *>(list->data);

    renderer->children_.append(child);
    if (renderer->isTemplateRenderer(child))
        qobject_cast<TemplateRenderer *>(child)->setParentRenderer(renderer);
}

int TemplateRenderer::childrenCount(DeclarativeListProperty<QObject> *list)
{
    TemplateRenderer *renderer = static_cast<TemplateRenderer *>(list->data);
    return renderer->children_.count();
}

QObject *TemplateRenderer::child(DeclarativeListProperty<QObject> *list, int index)
{
    TemplateRenderer *renderer = static_cast<TemplateRenderer *>(list->data);
    return renderer->children_.at(index);
}

void TemplateRenderer::clearChildren(DeclarativeListProperty<QObject> *list)
{
    TemplateRenderer *renderer = static_cast<TemplateRenderer *>(list->data);
    renderer->children_.clear();
}
