#include <QApplication>
#include <QFile>
#include <QIcon>
#include <QDebug>
#include "MainWindow.h"

int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    QApplication app(argc, argv);
    app.setApplicationName("Sorta");
    app.setApplicationVersion("1.0.0");
    app.setOrganizationName("Abdullah Bhatti");
    app.setOrganizationDomain("github.com/1abdullahr1");
    app.setWindowIcon(QIcon(":/app.png"));

    // Load modern stylesheet
    QFile styleFile(":/styles/modern.qss");
    if (styleFile.open(QFile::ReadOnly | QFile::Text)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        app.setStyleSheet(styleSheet);
        styleFile.close();
    }

    MainWindow mainWindow;
    mainWindow.show();

    return app.exec();
}
