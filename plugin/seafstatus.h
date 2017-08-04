#ifndef SEAFSTATUS_H
#define SEAFSTATUS_H

#include <QObject>
#include <QHash>
#include <QUuid>
#include <QException>
extern "C" {
#include <ccnet.h>
}

class SeafException : public QException
{
public:
	SeafException(GError *error);
	SeafException(int code, QByteArray message);

	const char *what() const noexcept override;

public:
	void raise() const override;
	QException *clone() const override;

private:
	int _code;
	QByteArray _message;
};

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

	void connect();
	void disconnect();

	SyncStatus syncStatus(const QString &path);

private:
	CcnetClientPool *_pool;
	SearpcClient *_client;

	QHash<QString, QUuid> _repoIds;

	void loadRepos();
};

#endif // SEAFSTATUS_H
