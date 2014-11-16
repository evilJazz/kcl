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
