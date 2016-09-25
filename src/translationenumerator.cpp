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

#include "KCL/translationenumerator.h"

#include <QDir>
#include <QLocale>

#include "KCL/debug.h"

TranslationEnumerator::TranslationEnumerator(const QStringList &translationPaths, QObject *parent) :
    QObject(parent),
    translationPaths_(translationPaths)
{
}

void TranslationEnumerator::updateList()
{
    availableTranslations_.clear();

    foreach (QString translationPath, translationPaths_)
    {
        QDir dir(translationPath);
        QStringList fileNames = dir.entryList(QStringList("*.qm"), QDir::Files, QDir::Name);

        foreach (QString qmFile, fileNames)
        {
            QString languageAbbr = QString::null;
            QRegExp re("[_-]([a-z]{2,2})([_-][A-Z]{2,3}){0,1}.qm$");

            if (re.indexIn(qmFile) > -1)
            {
                QVariantMap translation;
                translation.insert("qmFile", dir.filePath(qmFile));

                QStringList caps = re.capturedTexts();

                languageAbbr = caps.at(1);
                languageAbbr += caps.at(2);

                translation.insert("languageAbbr", languageAbbr);

                QLocale locale = QLocale(languageAbbr);
                translation.insert("languageName", QLocale::languageToString(locale.language()));
                //translation.insert("languageNativeName", locale.nativeLanguageName());

                DPRINTF("languageAbbrev: %s, languageName: %s",
                    translation["languageAbbrev"].toString().toUtf8().constData(),
                    translation["languageName"].toString().toUtf8().constData()
                );

                availableTranslations_.append(translation);
            }
        }
    }

    emit availableTranslationsChanged();
}

