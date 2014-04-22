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
    sceneutils \
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
kclConditionalAddModule(objectutils)
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

    kclConditionalAddModule(declarativedebug)
}

kcl_qtquick1 | kcl_qtquick2 {

    HEADERS += $$KCL_INC_PATH/kclplugin.h
    SOURCES += $$KCL_SRC_PATH/kclplugin.cpp

    kclConditionalAddModule(settingsgroup)
    kclConditionalAddModule(cursorarea)
    kclConditionalAddModule(imagerendersurface)
    kclConditionalAddModule(sceneutils)

    OTHER_FILES += \
        $$KCL_SRC_PATH/qml/Tests/SplitterTest/SplitterTest.qml \
        $$KCL_SRC_PATH/qml/Tests/SplitterTest/SplitterTest.qmlproject \
        $$KCL_SRC_PATH/qml/Tests/SplitterTest/SplitterTest2.qml \
        $$KCL_SRC_PATH/qml/Tests/ConstrainedText/ConstrainedTextTest.qml \
        $$KCL_SRC_PATH/qml/KCL/QueueManager.js \
        $$KCL_SRC_PATH/qml/KCL/MessageOverlay.js \
        $$KCL_SRC_PATH/qml/KCL/Splitter.qml \
        $$KCL_SRC_PATH/qml/KCL/RangeLogic.qml \
        $$KCL_SRC_PATH/qml/KCL/QueueManager.qml \
        $$KCL_SRC_PATH/qml/KCL/QtObjectWithChildren.qml \
        $$KCL_SRC_PATH/qml/KCL/MessageOverlay.qml \
        $$KCL_SRC_PATH/qml/KCL/FlickableColumn.qml \
        $$KCL_SRC_PATH/qml/KCL/Fader.qml \
        $$KCL_SRC_PATH/qml/KCL/ExtListView.qml \
        $$KCL_SRC_PATH/qml/KCL/ConstrainedText.qml \
        $$KCL_SRC_PATH/qml/KCL/CommandList.qml \
        $$KCL_SRC_PATH/qml/KCL/Command.qml \
        $$KCL_SRC_PATH/qml/KCL/BidirectionalBinding.qml \
        $$KCL_SRC_PATH/qml/KCL/AutoRow.qml \
        $$KCL_SRC_PATH/qml/KCL/AutoColumn.qml \
        $$KCL_SRC_PATH/qml/KCL/ActionList.qml \
        $$KCL_SRC_PATH/qml/KCL/ActionForwarder.qml \
        $$KCL_SRC_PATH/qml/KCL/ActionCommand.qml \
        $$KCL_SRC_PATH/qml/KCL/ActionBinding.qml \
        $$KCL_SRC_PATH/qml/KCL/Action.qml \
        $$KCL_SRC_PATH/qml/KCL/WebCall.js \
        $$KCL_SRC_PATH/qml/KCL/StringHelpers.js \
        $$KCL_SRC_PATH/qml/KCL/NumericHelpers.js \
        $$KCL_SRC_PATH/qml/KCL/JSONHelpers.js \
        $$KCL_SRC_PATH/qml/KCL/DeferredExecution.js \
        $$KCL_SRC_PATH/qml/KCL/ArrayHelpers.js \
        $$KCL_SRC_PATH/qml/KCL/private/SplitterMouseArea.qml \
        $$KCL_SRC_PATH/qml/KCL/private/SplitterLogic.qml \
        $$KCL_SRC_PATH/qml/KCL/private/RowColumnAutoSizeLogic.qml \
        $$KCL_SRC_PATH/qml/KCL/private/RangeLogic.qml \
        $$KCL_SRC_PATH/qml/KCL/qmldir \
        $$KCL_SRC_PATH/qml/KCL/MessageBox.js \
        $$KCL_SRC_PATH/qml/KCL/MessageBox.qml \
        $$KCL_SRC_PATH/qml/KCL/FileDialog.qml \
        $$KCL_SRC_PATH/qml/KCL/ValueChangeWatcher.qml \
        $$KCL_SRC_PATH/qml/KCL/MouseEater.qml

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
