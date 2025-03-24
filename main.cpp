#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "snake.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);

    QQmlApplicationEngine engine;
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    qmlRegisterType<Snake>("SnakeGame", 1, 0, "Snake");
    engine.loadFromModule("Snake-Game", "Main");

    return app.exec();
}
