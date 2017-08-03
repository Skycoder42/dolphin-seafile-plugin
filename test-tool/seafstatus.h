#ifndef SEAFSTATUS_H
#define SEAFSTATUS_H

#include <QObject>
#include <QHash>
#include <QUuid>
extern "C" {
#include <ccnet.h>
}

class SeafStatus : public QObject
{
	Q_OBJECT

public:
	explicit SeafStatus(QObject *parent = nullptr);
	~SeafStatus();

	QByteArray syncStatus(const QString &path);

private:
	CcnetClientPool *_pool;
	SearpcClient *_client;

	QHash<QString, QUuid> _repoIds;

	void loadRepos();
};

#endif // SEAFSTATUS_H
