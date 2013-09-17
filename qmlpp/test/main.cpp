#if defined(TARGET_QTQUICK1)
    #include <QApplication>
    #include "qmlapplicationviewer.h"
#elif defined(TARGET_QTQUICK2)
    #include <QtGui/QGuiApplication>
    #include "qtquick2applicationviewer.h"
#endif

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

    viewer.setMainQmlFile(QLatin1String("qml/qmlpptest/main.qml"));
    viewer.showExpanded();

    return app.exec();
}
