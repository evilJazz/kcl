contains(QT_VERSION, ^4\\.[0-6]\\..*) {
    message("KCL: Cannot build using Qt version $${QT_VERSION}.")
    error("Use at least Qt 4.7.")
}

QT += network

contains(QT_VERSION, ^4\\..*) {
    CONFIG += kcl_qt4

    contains(QT, declarative): CONFIG += kcl_qtquick1
    contains(QT, gui): CONFIG += kcl_widgets

    message("KCL: Configuring for Qt 4, actual version: $${QT_VERSION}")
}

contains(QT_VERSION, ^5\\..*) {
    CONFIG += kcl_qt5
    QT += concurrent

    contains(QT, quick) {
        CONFIG += kcl_qtquick2
        DEFINES += KCL_QTQUICK2
    }

    contains(QT, declarative): CONFIG += kcl_qtquick1
    contains(QT, widgets): CONFIG += kcl_widgets

    message("KCL: Configuring for Qt 5, actual version: $${QT_VERSION}")
}

KCL_SRC_PATH = $${PWD}/src
KCL_INC_PATH = $${PWD}/include

isEmpty(kcl): kcl = \
    binaryfiledownloader \
    filescanner \
    filesystemutils \
    history \
    progressmanager \
    performancedatamanager \
    keyeventfilter \
    imagefastloader \
    logging \
    debug \
    qobjectlistmodel \
    translationenumerator \
    sleepaid \
    backgroundtasks \
    networkutils \
    colorutils \
    graphicsutils \
    objectutils \
    nativedialogs \
    declarativedebug \
    settingsgroup \
    cursorarea \
    imagerendersurface \
    imageutils \
    sceneutils \
    datetimeutils \
    base64imageprovider

defineTest(kclAddModule) {
    message(KCL: Adding module $${1})
    HEADERS += $$KCL_INC_PATH/KCL/$${1}.h
    exists($$KCL_SRC_PATH/$${1}.cpp): SOURCES += $$KCL_SRC_PATH/$${1}.cpp
    DEFINES += KCL_$${1}
    export(HEADERS)
    export(SOURCES)
    export(DEFINES)
}

defineTest(kclConditionalAddModule) {
    contains(kcl, $${1}): kclAddModule($${1})
}

INCLUDEPATH += $$KCL_INC_PATH
HEADERS     += $$KCL_INC_PATH/KCL/kcl_global.h

kclConditionalAddModule(binaryfiledownloader)
kclConditionalAddModule(filescanner)
kclConditionalAddModule(filesystemutils)
kclConditionalAddModule(history)
kclConditionalAddModule(progressmanager)
kclConditionalAddModule(performancedatamanager)
kclConditionalAddModule(keyeventfilter)
kclConditionalAddModule(imagefastloader)
kclConditionalAddModule(logging)
kclConditionalAddModule(debug)
kclConditionalAddModule(qobjectlistmodel)
kclConditionalAddModule(translationenumerator)
kclConditionalAddModule(sleepaid)
kclConditionalAddModule(backgroundtasks)
kclConditionalAddModule(networkutils)
kclConditionalAddModule(colorutils)
kclConditionalAddModule(graphicsutils)
kclConditionalAddModule(imageutils)
kclConditionalAddModule(objectutils)
kclConditionalAddModule(datetimeutils)
kclConditionalAddModule(base64imageprovider)

kcl_widgets {
    message("KCL: Configuring with Widgets support")

    DEFINES += KCL_WIDGETS

    kclConditionalAddModule(nativedialogs)
}

kcl_qtquick1 {
    message("KCL: Configuring with QtQuick 1.x support")

    QT += script

    DEFINES -= KCL_QTQUICK2
}

kcl_qtquick1 | kcl_qtquick2 {

    HEADERS += $$KCL_INC_PATH/kclplugin.h
    SOURCES += $$KCL_SRC_PATH/kclplugin.cpp

    kclConditionalAddModule(settingsgroup)
    kclConditionalAddModule(cursorarea)
    kclConditionalAddModule(imagerendersurface)
    kclConditionalAddModule(sceneutils)
    kclConditionalAddModule(declarativedebug)

    OTHER_FILES += $$files($$KCL_SRC_PATH/qml/KCL/*, true)

    RESOURCES += \
        $$KCL_SRC_PATH/qml/KCL/kcl-qml.qrc

    QML_IMPORT_PATH += \
        $$KCL_SRC_PATH/qml/

    !kcl_noqmlpp {
        include(qmlpp/src/qmlpp.pri)

        kcl_qtquick1: qmlPreprocessFolder($$KCL_SRC_PATH/qml, @QtQuick1, 1.0)
        kcl_qtquick2: qmlPreprocessFolder($$KCL_SRC_PATH/qml, @QtQuick2, 2.0)
    }
}

