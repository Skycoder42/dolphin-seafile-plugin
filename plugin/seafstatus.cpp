#include "seafstatus.h"
#include <QGlobalStatic>
#include <QDebug>
#include <QDir>
extern "C" {
#include <seafile/seafile-object.h>
#include <seafile/seafile.h>
}

struct CCnetHelper
{
	inline CCnetHelper() :
		_pool(ccnet_client_pool_new(nullptr, DEFAULT_CONFIG_DIR))
	{}
	inline ~CCnetHelper() {
		free(_pool);
	}

	inline CcnetClientPool *clientPool() const {
		return _pool;
	}

private:
	CcnetClientPool *_pool;
};
Q_GLOBAL_STATIC(CCnetHelper, ccnet)

static QHash<QByteArray, SeafStatus::SyncStatus> path_status = {
	{"none", SeafStatus::None},
	{"syncing", SeafStatus::Syncing},
	{"error", SeafStatus::Error},
	{"ignored", SeafStatus::Ignored},
	{"synced", SeafStatus::Synced},
	{"paused", SeafStatus::Paused},
	{"readonly", SeafStatus::Readonly},
	{"locked", SeafStatus::Locked},
	{"locked_by_me", SeafStatus::LockedByMe}
};

SeafStatus::SeafStatus(QObject *parent) :
	QObject(parent),
	_client(nullptr),
	_repoIds(),
	_conTimer(new QTimer(this))
{
	_conTimer->setTimerType(Qt::VeryCoarseTimer);
	_conTimer->setInterval(60 * 1000);//1 minute

	connect(_conTimer, &QTimer::timeout,
			this, &SeafStatus::freeConnection);
}

SeafStatus::~SeafStatus()
{
	freeConnection();
}

void SeafStatus::engage()
{
	ensureConnected();
	_conTimer->stop();
}

void SeafStatus::disengage()
{
	_conTimer->start();
}

void SeafStatus::reloadRepos()
{
	ensureConnected();
	_repoIds.clear();

	GError *error = NULL;
	auto res = seafile_get_repo_list(_client, 0, -1, &error);
	if(error)
		throw SeafException(error);
	else {
		for (auto l = res; l != NULL; l = l->next) {
			auto repo = SEAFILE_REPO(l->data);
			_repoIds.insert(QString::fromUtf8(seafile_repo_get_worktree(repo)),
							QUuid(seafile_repo_get_id(repo)));
		}
	}
	g_list_free(res);
}

bool SeafStatus::hasRepo(const QString &path)
{
	return _repoIds.contains(repoPath(path));
}

SeafStatus::SyncStatus SeafStatus::syncStatus(const QString &path)
{
	ensureConnected();
	//TODO handle repository folder name

	auto repo = repoPath(path);
	if(!repo.isNull()) {
		QDir repoDir(repo);
		auto id = _repoIds[repo];
		auto idString = id.toByteArray();
		idString = idString.mid(1, idString.size() - 2);

		GError *error = NULL;
		auto res = searpc_client_call__string(_client, "seafile_get_path_sync_status", &error, 3,
											  "string", idString.constData(),
											  "string", repoDir.relativeFilePath(path).toUtf8().constData(),
											  "int", QFileInfo(path).isDir());
		if(error)
			throw SeafException(error);
		else {
			auto status = path_status.value(res);
			free(res);
			return status;
		}
	}

	return None;
}

void SeafStatus::ensureConnected()
{
	if(!_client) {
		auto pool = ccnet->clientPool();
		if(!pool)
			throw SeafException(tr("Ccnet pool unavailable - make shure ccnet is running"));

		_client = ccnet_create_pooled_rpc_client(pool, nullptr, "seafile-rpcserver");
		if(!_client)
			throw SeafException(tr("Unable to create ccnet rpc client for seafile-rpcserver"));

		reloadRepos();
	}

	_conTimer->start();
}

void SeafStatus::freeConnection()
{
	if(_client){
		ccnet_rpc_client_free(_client);
		_client = nullptr;
	}
}

QString SeafStatus::repoPath(const QString &path)
{
	QFileInfo info(path);
	auto fullPath = QDir::cleanPath(info.absoluteFilePath());
	foreach (auto repo, _repoIds.keys()) {
		if(fullPath.startsWith(repo))
			return repo;
	}

	return QString();
}



SeafException::SeafException(GError *error) :
	SeafException(SeafStatus::tr("Seafile Extension: %1").arg(QString::fromUtf8(error->message)).toUtf8())
{
	g_free(error);
}

SeafException::SeafException(QString message) :
	QException(),
	_message(SeafStatus::tr("Seafile Extension: %1").arg(message).toUtf8())
{}

const char *SeafException::what() const noexcept
{
	return _message.constData();
}

void SeafException::raise() const
{
	throw *this;
}

QException *SeafException::clone() const
{
	return new SeafException(_message);
}

SeafException::SeafException(QByteArray message) :
	QException(),
	_message(message)
{}
