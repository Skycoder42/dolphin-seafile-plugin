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
	enum SyncStatus {
		None = 0,
		Syncing,
		Error,
		Ignored,
		Synced,
		Paused,
		Readonly,
		Locked,
		LockedByMe,
		Invalid
	};
	Q_ENUM(SyncStatus)

	explicit SeafStatus(QObject *parent = nullptr);
	~SeafStatus();

	SyncStatus syncStatus(const QString &path);

private:
	CcnetClientPool *_pool;
	SearpcClient *_client;

	QHash<QString, QUuid> _repoIds;

	void loadRepos();
};

#endif // SEAFSTATUS_H
