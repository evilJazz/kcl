#if defined(TARGET_QTQUICK1)
    #include <QApplication>
    #include "qmlapplicationviewer.h"
    #include <QDeclarativeEngine>
#elif defined(TARGET_QTQUICK2)
    #include <QtGui/QGuiApplication>
    #include "qtquick2applicationviewer.h"
    #include <QQmlEngine>
#endif

#include "kclplugin.h"

#include "testimageprovider.h"

Q_DECL_EXPORT int main(int argc, char *argv[])
{
#if defined(TARGET_QTQUICK1)
    QScopedPointer<QApplication> appPtr(createApplication(argc, argv));
    QApplication &app = *appPtr;

    QmlApplicationViewer viewer;
    viewer.addImportPath(QLatin1String("modules"));
    viewer.setOrientation(QmlApplicationViewer::ScreenOrientationAuto);
#elif defined(TARGET_QTQUICK2)
    QGuiApplication app(argc, argv);

    QtQuick2ApplicationViewer viewer;
#endif

    KCLPlugin kclPlugin;
    kclPlugin.registerTypes("KCL");
    kclPlugin.initializeEngine(viewer.engine(), "KCL");

    viewer.engine()->addImageProvider("test", new TestImageProvider());

    viewer.setMainQmlFile(QLatin1String("qml/ThumbnailExpress/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
