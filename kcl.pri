contains(QT_VERSION, ^4\\.[0-6]\\..*) {
    message("KCL: Cannot build using Qt version $${QT_VERSION}.")
    error("Use at least Qt 4.7.")
}

QT += network

contains(QT_VERSION, ^4\\..*) {
    CONFIG += kcl_qt4 kcl_qtquick1 kcl_widgets
    QT += declarative
    message("KCL: Configuring for Qt 4, actual version: $${QT_VERSION}")
}

contains(QT_VERSION, ^5\\..*) {
    CONFIG += kcl_qt5
    QT += concurrent

    contains(QT, declarative) {
        CONFIG += kcl_qtquick1
    }

    contains(QT, quick) {
        CONFIG += kcl_qtquick2
        DEFINES += KCL_QTQUICK2
    }

    contains(QT, widgets): CONFIG += kcl_widgets

    message("KCL: Configuring for Qt 5, actual version: $${QT_VERSION}")
}

QT += script

KCL_SRC_PATH = $${PWD}/src
KCL_INC_PATH = $${PWD}/include

INCLUDEPATH += \
    $$KCL_INC_PATH

HEADERS += \
    $$KCL_INC_PATH/kclplugin.h \
    $$KCL_INC_PATH/KCL/kcl_global.h \
    $$KCL_INC_PATH/KCL/binaryfiledownloader.h \
    $$KCL_INC_PATH/KCL/filescanner.h \
    $$KCL_INC_PATH/KCL/filesystemutils.h \
    $$KCL_INC_PATH/KCL/settingsgroup.h \
    $$KCL_INC_PATH/KCL/history.h \
    $$KCL_INC_PATH/KCL/progressmanager.h \
    $$KCL_INC_PATH/KCL/performancedatamanager.h \
    $$KCL_INC_PATH/KCL/keyeventfilter.h \
    $$KCL_INC_PATH/KCL/imagefastloader.h \
    $$KCL_INC_PATH/KCL/cursorarea.h \
    $$KCL_INC_PATH/KCL/logging.h \
    $$KCL_INC_PATH/KCL/debug.h \
    $$KCL_INC_PATH/KCL/qobjectlistmodel.h \
    $$KCL_INC_PATH/KCL/translationenumerator.h \
    $$KCL_INC_PATH/KCL/sleepaid.h \
    $$KCL_INC_PATH/KCL/backgroundtasks.h \
    $$KCL_INC_PATH/KCL/imagerendersurface.h

SOURCES += \
    $$KCL_SRC_PATH/kclplugin.cpp \
    $$KCL_SRC_PATH/binaryfiledownloader.cpp \
    $$KCL_SRC_PATH/filescanner.cpp \
    $$KCL_SRC_PATH/filesystemutils.cpp \
    $$KCL_SRC_PATH/settingsgroup.cpp \
    $$KCL_SRC_PATH/history.cpp \
    $$KCL_SRC_PATH/progressmanager.cpp \
    $$KCL_SRC_PATH/performancedatamanager.cpp \
    $$KCL_SRC_PATH/keyeventfilter.cpp \
    $$KCL_SRC_PATH/imagefastloader.cpp \
    $$KCL_SRC_PATH/cursorarea.cpp \
    $$KCL_SRC_PATH/logging.cpp \
    $$KCL_SRC_PATH/debug.cpp \
    $$KCL_SRC_PATH/translationenumerator.cpp \
    $$KCL_SRC_PATH/backgroundtasks.cpp \
    $$KCL_SRC_PATH/imagerendersurface.cpp

kcl_widgets {
    message("KCL: Configuring with Widgets support")

    DEFINES += KCL_WIDGETS

    HEADERS += \
        $$KCL_INC_PATH/KCL/nativedialogs.h

    SOURCES += \
        $$KCL_SRC_PATH/nativedialogs.cpp
}

kcl_qtquick1 {
    message("KCL: Configuring with QtQuick 1.x support")

    DEFINES -= KCL_QTQUICK2

    HEADERS += \
        $$KCL_INC_PATH/KCL/declarativedebug.h

    SOURCES += \
        $$KCL_SRC_PATH/declarativedebug.cpp
}

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
