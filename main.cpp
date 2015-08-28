#include <QtGui>
#include <QtQml>
#include <QtQuick>
#include <QQmlEngine>
#include "gamelogic.h"

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QUrl source = QUrl(QStringLiteral("qrc:///main.qml"));
	QQmlApplicationEngine engine;
    
    qmlRegisterType<GameLogic>("com.Kanari.GameLogic", 1, 0, "GameLogic");
    
	engine.load(source);

	return app.exec();
}
