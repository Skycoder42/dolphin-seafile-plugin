#include <QCoreApplication>
#include <QPluginLoader>
#include <QDebug>

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	QPluginLoader loader("/home/sky/Programming/QtProjects/build-dolphin-seafile-plugin-System_Qt_5_9_1_Arch-Debug/plugin/libfileviewseafileplugin.so");
	qDebug() << loader.metaData();
	qDebug() << loader.load();
	qDebug() << loader.errorString();

	return 0;
}
