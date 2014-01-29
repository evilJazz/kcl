#ifndef TRANSLATIONENUMERATOR_H
#define TRANSLATIONENUMERATOR_H

#include "KCL/kcl_global.h"

#include <QObject>
#include <QVariant>

class KCL_EXPORT TranslationEnumerator : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QVariantList availableTranslations READ availableTranslations NOTIFY availableTranslationsChanged)
public:
    explicit TranslationEnumerator(const QString translationPath = ":/i18n", QObject *parent = 0);

    QVariantList availableTranslations() const { return availableTranslations_; }

    Q_INVOKABLE void updateList();

    const QString translationPath() const { return translationPath_; }
    void setTranslationPath(const QString &translationPath) { translationPath_ = translationPath; }

signals:
    void availableTranslationsChanged();

public slots:

private:
    QString translationPath_;
    QVariantList availableTranslations_;
};

#endif // TRANSLATIONENUMERATOR_H
