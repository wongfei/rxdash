#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <csignal>

#include "DashDriver.h"
#include "utils.h"

void sigHandler(int sig) {
    qInfo() << "SIGNAL" << sig;
    QCoreApplication::exit(0);
}

int main(int argc, char *argv[]) {
    qInfo() << "enter main";
    int rc = 0;
    try {
        DashDriver::staticInit();

        QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
        QGuiApplication app(argc, argv);

        qmlRegisterType<Pfc>("rxdash", 1, 0, "Pfc");
        qmlRegisterType<DashDriver>("rxdash", 1, 0, "DashDriver");

        QQmlApplicationEngine engine;
        engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

        signal(SIGINT, sigHandler);
        signal(SIGTERM, sigHandler);
        rc = app.exec();

    } catch (const std::exception& ex) {
        qCritical() << "main: exception:" << ex.what();
    } catch (...) {
        qCritical() << "main: unknown exception";
    }
    qInfo() << "exit";
    if (rc == DashDriver::RC_POWEROFF) {
        ut::osPowerOff();
    }
    return rc;
}
