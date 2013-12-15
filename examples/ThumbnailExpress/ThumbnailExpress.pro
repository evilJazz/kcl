# Add more folders to ship with the application, here
folder_01.source = qml/ThumbnailExpress
folder_01.target = qml
DEPLOYMENTFOLDERS = folder_01

# Additional import path used to resolve QML modules in Creator's code model
QML_IMPORT_PATH =

# If your application uses the Qt Mobility libraries, uncomment the following
# lines and add the respective components to the MOBILITY variable.
# CONFIG += mobility
# MOBILITY +=

# Speed up launching on MeeGo/Harmattan when using applauncherd daemon
# CONFIG += qdeclarative-boostable

# The .cpp file which was generated for your project. Feel free to hack it.
SOURCES += main.cpp \
    testimageprovider.cpp

HEADERS += \
    testimageprovider.h

# Installation path
# target.path =

## -- QMLPP RELATED CHANGES ARE BELOW -- #

# Include QML preprocessor
include(../../qmlpp/src/qmlpp.pri)

# Check which version of Qt we are dealing with and set up targets...
contains(QT_VERSION, ^4\\..*): CONFIG += target_qtquick1
contains(QT_VERSION, ^5\\..*): CONFIG += target_qtquick2

target_qtquick1 {
    # Call preprocessor on all files in qml/qmlpptest, set defines @QtQuick1 and @JavaScriptTest1
    # and rewrite QtQuick import to version 1.1
    qmlPreprocessFolder($$PWD/qml/ThumbnailExpress, @QtQuick1 @JavaScriptTest1, 1.1)

    # Set define required for combined viewer code in main.cpp
    DEFINES += TARGET_QTQUICK1
    QT += declarative

    # Please do not modify the following two lines. Required for deployment.
    include(qmlapplicationviewer/qmlapplicationviewer.pri)
    qtcAddDeployment()
}
else: target_qtquick2 {
    # Call preprocessor on all files in qml/qmlpptest, set define @QtQuick2 and @JavaScriptTest2
    # and rewrite QtQuick import to version 2.1
    qmlPreprocessFolder($$PWD/qml/ThumbnailExpress, @QtQuick2 @JavaScriptTest2, 2.1)

    # Set define required for combined viewer code in main.cpp
    DEFINES += TARGET_QTQUICK2
    QT += quick

    # Please do not modify the following two lines. Required for deployment.
    include(qtquick2applicationviewer/qtquick2applicationviewer.pri)
    qtcAddDeployment()
}

include(../../kcl.pri)
