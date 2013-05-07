KCL_SRC_PATH = $${PWD}/src

INCLUDEPATH += \
    $$KCL_SRC_PATH

HEADERS += \
    $$KCL_SRC_PATH/binaryfiledownloader.h \
    $$KCL_SRC_PATH/filescanner.h \
    $$KCL_SRC_PATH/filesystemutils.h \
    $$KCL_SRC_PATH/settingsgroup.h \
    $$KCL_SRC_PATH/kclplugin.h \
    $$KCL_SRC_PATH/progressmanager.h \
    $$KCL_SRC_PATH/performancedatamanager.h \
    $$KCL_SRC_PATH/logging.h \
    $$KCL_SRC_PATH/keyeventfilter.h \
    $$KCL_SRC_PATH/imagefastloader.h \
    $$KCL_SRC_PATH/debug.h

SOURCES += \
    $$KCL_SRC_PATH/binaryfiledownloader.cpp \
    $$KCL_SRC_PATH/filescanner.cpp \
    $$KCL_SRC_PATH/filesystemutils.cpp \
    $$KCL_SRC_PATH/settingsgroup.cpp \
    $$KCL_SRC_PATH/kclplugin.cpp \
    $$KCL_SRC_PATH/progressmanager.cpp \
    $$KCL_SRC_PATH/performancedatamanager.cpp \
    $$KCL_SRC_PATH/logging.cpp \
    $$KCL_SRC_PATH/keyeventfilter.cpp \
    $$KCL_SRC_PATH/imagefastloader.cpp \
    $$KCL_SRC_PATH/debug.cpp

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
    $$KCL_SRC_PATH/qml/KCL/private/RangeLogic.qml

RESOURCES += \
    $$KCL_SRC_PATH/qml/KCL/kcl-qml.qrc

QML_IMPORT_PATH += \
    $$KCL_SRC_PATH/qml/
