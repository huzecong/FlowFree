#include <QtGui>
#include <QtQml>
#include <QtQuick>

int main(int argc, char *argv[])
{
	QGuiApplication app(argc, argv);

	QUrl source = QUrl(QStringLiteral("qrc:///main.qml"));
	QQmlApplicationEngine engine;
	engine.load(source);

	return app.exec();
}
