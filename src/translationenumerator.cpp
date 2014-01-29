#include "KCL/translationenumerator.h"

#include <QDir>
#include <QLocale>

#include "KCL/debug.h"

KCL_USE_NAMESPACE

TranslationEnumerator::TranslationEnumerator(const QString translationPath, QObject *parent) :
    QObject(parent),
    translationPath_(translationPath)
{
}

void TranslationEnumerator::updateList()
{
    availableTranslations_.clear();

    QDir dir(translationPath_);
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

    emit availableTranslationsChanged();
}

