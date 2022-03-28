#include "dtlslink.h"
#include "common.h"
#include "configmanager.h"
#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QTranslator>
#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WIN64)
#include "singleapplication.h"
#endif


int main(int argc, char *argv[])
{
    qSetMessagePattern("%{type}:[%{file}:%{line}]  %{message}");
    QApplication::setApplicationName("DtlsLink");
    configLocation = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    tempLocation = QStandardPaths::writableLocation(QStandardPaths::TempLocation);
    QDir().mkpath(configLocation);

#if defined(Q_OS_LINUX) || defined(Q_OS_MACOS) || defined(Q_OS_WIN64)
    SingleApplication app(argc, argv);
#else
    QApplication app(argc, argv);
#endif
    configManager = new ConfigManager();
    // Multiple translation files can be installed.
    // Translations are searched for in the reverse order in which they were installed
    QTranslator myTranslator; // must global

    if(configManager->loadConfig(Json)) {
        if(configManager->config["local"].toBool()) {
            // when debug mode,always en_US
//            qDebug() << QLocale::system().name();
            if(myTranslator.load(QLocale(), QLatin1String("dtlslink"), QLatin1String("_"), QLatin1String(":/i18n"))) {
                app.installTranslator(&myTranslator);
            }
        }
    }

    DtlsLink w;
    w.show();

    QApplication::setQuitOnLastWindowClosed(false);
    QObject::connect(&app, &SingleApplication::instanceStarted, &w, &DtlsLink::show);

    return app.exec();
}
