#include <QCoreApplication>
#include <QDebug>

#include "seafstatus.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SeafStatus status;
	auto args = a.arguments();
	args.removeFirst();
	foreach(auto arg, args)
		qInfo() << arg << status.syncStatus(arg);
	return 0;
}
