# Enable if you want to debug KCL...
#DEFINES += KCL_DEBUG

contains(QT_VERSION, ^4\\.[0-6]\\..*) {
    message("KCL: Cannot build using Qt version $${QT_VERSION}.")
    error("Use at least Qt 4.7.")
}

QT += network

contains(QT_VERSION, ^4\\..*) {
    CONFIG += kcl_qt4

    contains(QT, declarative): CONFIG += kcl_qtquick1 kcl_declarative
    contains(QT, gui): CONFIG += kcl_widgets

    message("KCL: Configuring for Qt 4, actual version: $${QT_VERSION}")
}

contains(QT_VERSION, ^5\\..*) {
    CONFIG += kcl_qt5
    QT += concurrent

    contains(QT, quick) {
        CONFIG += kcl_qtquick2 kcl_declarative
        DEFINES += KCL_QTQUICK2
    }

    contains(QT, declarative): CONFIG += kcl_qtquick1 kcl_declarative
    contains(QT, widgets): CONFIG += kcl_widgets

    message("KCL: Configuring for Qt 5, actual version: $${QT_VERSION}")
}

KCL_SRC_PATH = $${PWD}/src
KCL_INC_PATH = $${PWD}/include

isEmpty(kcl): kcl = \
    binaryfiledownloader \
    cookiejarinterface \
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
    rawmousearea \
    datetimeutils \
    base64imageprovider \
    updatelocker \
    sortfiltermodel \
    singletons \
    $$kclAdd

!isEmpty(kclRemove): kcl -= $$kclRemove

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

kclConditionalAddModule(filescanner)
kclConditionalAddModule(filesystemutils)
kclConditionalAddModule(history)
kclConditionalAddModule(progressmanager)
kclConditionalAddModule(performancedatamanager)
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
kclConditionalAddModule(updatelocker)
kclConditionalAddModule(sortfiltermodel)
kclConditionalAddModule(paranoidretrier)
kclConditionalAddModule(singletons)

kcl_widgets {
    message("KCL: Configuring with Widgets support")

    DEFINES += KCL_WIDGETS

    kclConditionalAddModule(nativedialogs)
}

kcl_qtquick1 {
    message("KCL: Configuring with QtQuick 1.x support")

    QT += script

    DEFINES -= KCL_QTQUICK2

    kclConditionalAddModule(rawmousearea)
}

kcl_qtquick2 {
    message("KCL: Configuring with QtQuick 2.x support")
}

kcl_declarative {
    message("KCL: Configuring declarative support classes")

    HEADERS += $$KCL_INC_PATH/kclplugin.h
    SOURCES += $$KCL_SRC_PATH/kclplugin.cpp

    kclConditionalAddModule(settingsgroup)
    kclConditionalAddModule(cursorarea)
    kclConditionalAddModule(imagerendersurface)
    kclConditionalAddModule(sceneutils)
    kclConditionalAddModule(declarativedebug)
    kclConditionalAddModule(binaryfiledownloader)
    kclConditionalAddModule(cookiejarinterface)
    kclConditionalAddModule(keyeventfilter)
    kclConditionalAddModule(base64imageprovider)

    OTHER_FILES += $$files($$KCL_SRC_PATH/qml/KCL/*, true)

    RESOURCES += \
        $$KCL_SRC_PATH/qml/KCL/kcl-qml.qrc

    QML_IMPORT_PATH += \
        $$KCL_SRC_PATH/qml/

    !kcl_noqmlpp {
        include($${PWD}/qmlpp/src/qmlpp.pri)

        kcl_qtquick1: qmlPreprocessFolder($$KCL_SRC_PATH/qml, @QtQuick1, 1.0)
        kcl_qtquick2: qmlPreprocessFolder($$KCL_SRC_PATH/qml, @QtQuick2, 2.0)
    }
}

contains(kcl, paranoidretrier) {
    message("KCL: Paranoid Retrier class enabled, make sure to enable C++X0/C++11 support in your compiler flags.")
}
