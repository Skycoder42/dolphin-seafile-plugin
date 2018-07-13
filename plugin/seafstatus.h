#ifndef SEAFSTATUS_H
#define SEAFSTATUS_H

#include <QObject>
#include <QHash>
#include <QUuid>
#include <QException>
#include <QTimer>

extern "C" {
#include <searpc-client.h>
}

class SeafException : public QException
{
public:
	SeafException(GError *error);
	SeafException(const QString &message);

	const char *what() const noexcept override;

public:
	void raise() const override;
	QException *clone() const override;

private:
	SeafException(QByteArray message);

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
	~SeafStatus() override;

	void engage();
	void disengage();

	void reloadRepos();

	bool hasRepo(const QString &path);
	SyncStatus syncStatus(const QString &path);

private slots:
	void ensureConnected();
	void freeConnection();

private:
	SearpcClient *_client;
	QHash<QString, QUuid> _repoIds;
	QTimer *_conTimer;

	QString repoPath(const QString &path);
	SyncStatus mapFileStatus(const QByteArray &text);
	SyncStatus mapRepoStatus(const QByteArray &text);
	QString readSeafileIni();
};

#endif // SEAFSTATUS_H
