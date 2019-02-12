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

#ifndef TEMPLATERENDERER_H
#define TEMPLATERENDERER_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QMap>
#include <QPointer>
#include <QUrl>
#include <QTimer>

#ifdef KCL_QTQUICK2
    #include <QtQuick>
    #define DeclarativeListProperty QQmlListProperty
    #define DeclarativeParserStatus QQmlParserStatus
#else
    #include <QDeclarativeListProperty>
    #include <QDeclarativeParserStatus>
    #define DeclarativeListProperty QDeclarativeListProperty
    #define DeclarativeParserStatus QDeclarativeParserStatus
#endif

#include "KCL/propertychangeobserver.h"

class TemplateRenderer : public PropertyChangeObserver
{
    Q_OBJECT
    Q_PROPERTY(TemplateRenderer *parentRenderer READ parentRenderer WRITE setParentRenderer NOTIFY parentRendererChanged)
    Q_PROPERTY(TemplateRenderer *topLevelRenderer READ topLevelRenderer WRITE setTopLevelRenderer NOTIFY topLevelRendererChanged)

    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(QString identifier READ identifier NOTIFY identifierChanged)
    Q_PROPERTY(QString childPrefix READ childPrefix WRITE setChildPrefix NOTIFY childPrefixChanged)

    Q_PROPERTY(QString templateText READ templateText WRITE setTemplateText NOTIFY templateTextChanged)
    Q_PROPERTY(QUrl templateSource READ templateSource WRITE setTemplateSource NOTIFY templateSourceChanged)
    Q_PROPERTY(QString templateRegEx READ templateRegEx WRITE setTemplateRegEx NOTIFY templateRegExChanged)

    Q_PROPERTY(QStringList markersUsedInTemplate READ markersUsedInTemplate NOTIFY markersUsedInTemplateChanged)

    Q_PROPERTY(QString content READ content NOTIFY contentChanged)
    Q_PROPERTY(bool contentDirty READ contentDirty NOTIFY contentDirtyChanged)

    Q_PROPERTY(int renderDelay READ renderDelay WRITE setRenderDelay NOTIFY renderDelayChanged)

    Q_PROPERTY(QStringList _TemplateRenderer_ignoredPropertyNames READ _TemplateRenderer_ignoredPropertyNames CONSTANT)

#ifdef KCL_QTQUICK2
    Q_PROPERTY(QQmlListProperty<TemplateRenderer> subRenderers READ subRenderers NOTIFY subRenderersChanged)
    Q_PROPERTY(QQmlListProperty<QObject> children READ children)
    Q_INTERFACES(QQmlParserStatus)
#else
    Q_PROPERTY(QDeclarativeListProperty<TemplateRenderer> subRenderers READ subRenderers NOTIFY subRenderersChanged)
    Q_PROPERTY(QDeclarativeListProperty<QObject> children READ children)
    Q_INTERFACES(QDeclarativeParserStatus)
#endif

    Q_CLASSINFO("DefaultProperty", "children")
public:
    explicit TemplateRenderer(QObject *parent = 0);
    virtual ~TemplateRenderer();

    TemplateRenderer *parentRenderer() const;
    void setParentRenderer(TemplateRenderer *newParentRenderer);

    TemplateRenderer *topLevelRenderer() const;

    QString name() const;
    void setName(const QString &name);

    QString identifier() const;

    QString childPrefix() const;
    void setChildPrefix(const QString &childPrefix);

    QString templateText() const;
    void setTemplateText(const QString &templateText);

    QUrl templateSource() const;
    void setTemplateSource(const QUrl &url);

    QString templateRegEx() const;
    void setTemplateRegEx(const QString &templateRegEx);

    QStringList markersUsedInTemplate() const;

    QString content() const;
    bool contentDirty() const;

    int renderDelay() const;
    void setRenderDelay(int delay);

    Q_INVOKABLE static const QString createUniqueID();

    Q_INVOKABLE void updateContent();
    Q_INVOKABLE TemplateRenderer *findTemplateRendererByName(const QString &name);
    Q_INVOKABLE bool isTemplateRenderer(QObject *item);
    Q_INVOKABLE bool isMarkerUsedInTemplate(const QString &name) const;

    Q_INVOKABLE void dumpRendererStructure();

    Q_INVOKABLE virtual QString replaceMarkerForProperty(const QVariant &propertyName);
    Q_INVOKABLE virtual QString replaceMarkerForContent();

    // Methods to call super methods in overridden methods in QML
    Q_INVOKABLE QStringList _TemplateRenderer_ignoredPropertyNames() const;
    Q_INVOKABLE QString _TemplateRenderer_replaceMarkerForProperty(const QString &propertyName);
    Q_INVOKABLE QString _TemplateRenderer_replaceMarkerForContent();

    DeclarativeListProperty<QObject> children();

    DeclarativeListProperty<TemplateRenderer> subRenderers();

signals:
    void parentRendererChanged();
    void topLevelRendererChanged();
    void nameChanged();
    void identifierChanged();
    void childPrefixChanged();
    void templateTextChanged();
    void templateSourceChanged();
    void templateRegExChanged();
    void markersUsedInTemplateChanged();
    void contentChanged();
    void contentDirtyChanged();
    void renderDelayChanged();

    void subRendererTemplateChanged(TemplateRenderer *renderer);
    void subRenderersChanged();

private slots:
    void handleTemplateChanged();
    void handleParentChanged();

    void notifyParentRenderer();
    void markContentDirty();

    void handleParentRendererTopLevelRendererChanged();
    void handleParentRendererChildPrefixChanged();

private:
    QStringList ignoredPropertyNames_;

    QPointer<TemplateRenderer> parentRenderer_;

    QString name_;
    QString childPrefix_;
    QString templateText_;
    QUrl templateSource_;
    QString templateRegEx_;

    QMap<QString, int> markersUsedInTemplate_;

    QString content_;
    bool contentDirty_;

    int renderDelay_;
    QPointer<QTimer> renderTimer_;

    QList<QObject *> children_;
    static void appendChild(DeclarativeListProperty<QObject> *, QObject *);
    static int childrenCount(DeclarativeListProperty<QObject> *);
    static QObject *child(DeclarativeListProperty<QObject> *, int index);
    static void clearChildren(DeclarativeListProperty<QObject> *);

    void removeRenderer(TemplateRenderer *childRenderer);
    void addRenderer(TemplateRenderer *childRenderer);

    QMap<QString, QPointer<TemplateRenderer> > subRenderersMap_;
    QList<TemplateRenderer *> subRenderers_;
    QPointer<TemplateRenderer> topLevelRenderer_;

    void setTopLevelRenderer(TemplateRenderer *);

    void dumpRendererStructure(const QString &indent);
};

#endif // TEMPLATERENDERER_H

