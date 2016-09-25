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

#ifndef TRANSLATIONENUMERATOR_H
#define TRANSLATIONENUMERATOR_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>
#include <QStringList>

class KCL_EXPORT TranslationEnumerator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList availableTranslations READ availableTranslations NOTIFY availableTranslationsChanged)
public:
    explicit TranslationEnumerator(const QStringList &translationPaths, QObject *parent = 0);

    QVariantList availableTranslations() const { return availableTranslations_; }

    Q_INVOKABLE void updateList();

    const QStringList translationPaths() const { return translationPaths_; }
    void setTranslationPaths(const QStringList &translationPaths) { translationPaths_ = translationPaths; }

signals:
    void availableTranslationsChanged();

public slots:

private:
    QStringList translationPaths_;
    QVariantList availableTranslations_;
};

#endif // TRANSLATIONENUMERATOR_H
