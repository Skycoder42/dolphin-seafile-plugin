#include "seafstatus.h"
#include <QDebug>
#include <QDir>
extern "C" {
#include <seafile/seafile-object.h>
#include <seafile/seafile.h>
}

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
	_pool(nullptr),
	_client(nullptr),
	_repoIds()
{}

SeafStatus::~SeafStatus()
{
	disconnectCcnet();
}

void SeafStatus::connectCcnet()
{
	if(!_pool)
		_pool = ccnet_client_pool_new(nullptr, DEFAULT_CONFIG_DIR);
	if(!_client && _pool)
		_client = ccnet_create_pooled_rpc_client(_pool, nullptr, "seafile-rpcserver");

	if(!_client)
		throw SeafException(0, "Unable to create ccnet rpc client for seafile-rpcserver");

	loadRepos();
}

void SeafStatus::disconnectCcnet()
{
	_repoIds.clear();

	if(_client){
		ccnet_rpc_client_free(_client);
		_client = nullptr;
	}

	if(_pool) {
		free(_pool);
		_pool = nullptr;
	}
}

SeafStatus::SyncStatus SeafStatus::syncStatus(const QString &path)
{
	//TODO handle repository folder name

	QFileInfo info(path);
	auto fullPath = QDir::cleanPath(info.absoluteFilePath());
	foreach (auto repo, _repoIds.keys()) {
		if(fullPath.startsWith(repo)) {
			QDir repoDir(repo);
			auto id = _repoIds[repo];
			auto idString = id.toByteArray();
			idString = idString.mid(1, idString.size() - 2);

			GError *error = NULL;
			auto res = searpc_client_call__string(_client, "seafile_get_path_sync_status", &error, 3,
												  "string", idString.constData(),
												  "string", repoDir.relativeFilePath(path).toUtf8().constData(),
												  "int", info.isDir());
			if(error)
				throw SeafException(error);
			else {
				auto status = path_status.value(res);
				free(res);
				return status;
			}
		}
	}

	return None;
}

void SeafStatus::loadRepos()
{
	_repoIds.clear();

	GError *error = NULL;
	auto res = seafile_get_repo_list(_client, 0, -1, &error);
	if(error)
		throw SeafException(error);
	else {
		for (auto l = res; l != NULL; l = l->next) {
			auto repo = SEAFILE_REPO(l->data);
			_repoIds.insert(QString::fromUtf8(repo->_worktree),
							QUuid(repo->_id));
		}
	}
	g_list_free(res);
}



SeafException::SeafException(GError *error) :
	SeafException(error->code, error->message)
{
	g_free(error);
}

SeafException::SeafException(int code, QByteArray message) :
	QException(),
	_code(code),
	_message(message)
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
	return new SeafException(_code, _message);
}
