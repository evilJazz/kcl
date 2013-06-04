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
    $$KCL_INC_PATH/KCL/logging.h \
    $$KCL_INC_PATH/KCL/keyeventfilter.h \
    $$KCL_INC_PATH/KCL/imagefastloader.h \
    $$KCL_INC_PATH/KCL/nativedialogs.h \
    $$KCL_INC_PATH/KCL/cursorarea.h \
    $$KCL_INC_PATH/KCL/debug.h \
    $$KCL_INC_PATH/KCL/declarativedebug.h

SOURCES += \
    $$KCL_SRC_PATH/kclplugin.cpp \
    $$KCL_SRC_PATH/binaryfiledownloader.cpp \
    $$KCL_SRC_PATH/filescanner.cpp \
    $$KCL_SRC_PATH/filesystemutils.cpp \
    $$KCL_SRC_PATH/settingsgroup.cpp \
    $$KCL_SRC_PATH/history.cpp \
    $$KCL_SRC_PATH/progressmanager.cpp \
    $$KCL_SRC_PATH/performancedatamanager.cpp \
    $$KCL_SRC_PATH/logging.cpp \
    $$KCL_SRC_PATH/keyeventfilter.cpp \
    $$KCL_SRC_PATH/imagefastloader.cpp \
    $$KCL_SRC_PATH/nativedialogs.cpp \
    $$KCL_SRC_PATH/cursorarea.cpp \
    $$KCL_SRC_PATH/debug.cpp \
    $$KCL_SRC_PATH/declarativedebug.cpp

OTHER_FILES += \
    $$KCL_SRC_PATH/qml/Tests/SplitterTest/SplitterTest.qml \
    $$KCL_SRC_PATH/qml/Tests/SplitterTest/SplitterTest.qmlproject \
    $$KCL_SRC_PATH/qml/Tests/SplitterTest/SplitterTest2.qml \
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
    $$KCL_SRC_PATH/qml/KCL/FileDialog.qml

RESOURCES += \
    $$KCL_SRC_PATH/qml/KCL/kcl-qml.qrc

QML_IMPORT_PATH += \
    $$KCL_SRC_PATH/qml/
