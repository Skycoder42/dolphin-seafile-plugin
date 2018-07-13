#include "seafstatus.h"
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
extern "C" {
#include <searpc-client.h>
#include <searpc-named-pipe-transport.h>
#include <searpc.h>
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
	{"locked_by_me", SeafStatus::LockedByMe},
	{NULL, SeafStatus::None},
};

SeafStatus::SeafStatus(QObject *parent) :
	QObject(parent)
{
	createClient();
	Q_ASSERT(_client);
	loadRepos();
}

SeafStatus::~SeafStatus()
{
	if(_client)
		searpc_free_client_with_pipe_transport(_client);
}

SeafStatus::SyncStatus SeafStatus::syncStatus(const QString &path)
{
	QFileInfo info(path);
	auto fullPath = QDir::cleanPath(info.absoluteFilePath());
	for(const auto &repo : _repoIds.keys()) {
		if(fullPath.startsWith(repo)) {
			QDir repoDir(repo);
			auto id = _repoIds[repo];
			auto idString = id.toByteArray();
			idString = idString.mid(1, idString.size() - 2);

			GError *error = nullptr;
			auto res = searpc_client_call__string(_client, "seafile_get_path_sync_status", &error, 3,
												  "string", idString.constData(),
												  "string", repoDir.relativeFilePath(path).toUtf8().constData(),
												  "int", info.isDir());
			if(error) {
				qCritical() << error->message;
				g_error_free(error);
				return SeafStatus::Invalid;
			} else
				return path_status[res];
		}
	}

	return path_status[NULL];
}

bool SeafStatus::createClient()
{
	auto path = QDir{QStandardPaths::writableLocation(QStandardPaths::HomeLocation)}.absoluteFilePath(QStringLiteral("Seafile/.seafile-data/seafile.sock"));
	auto pipe_client = searpc_create_named_pipe_client(qUtf8Printable(path));
	int ret = searpc_named_pipe_client_connect(pipe_client);
	_client = searpc_client_with_named_pipe_transport(pipe_client, "seafile-rpcserver");
	if (ret < 0) {
	   searpc_free_client_with_pipe_transport(_client);
	   _client = nullptr;
	   return false;
   } else
		return true;
}

void SeafStatus::loadRepos()
{
	GError *error = nullptr;
	//auto res = searpc_client_call__objlist(client, "seafile_get_repo_list", SEAFILE_TYPE_REPO, &error, 2, "int", 0, "int", 5);
	auto res = seafile_get_repo_list(_client, 0, -1, &error);
	if(error) {
		qCritical() << error->message;
		g_error_free(error);
	} else {
		for (auto l = res; l != nullptr; l = l->next) {
			auto repo = SEAFILE_REPO(l->data);
			_repoIds.insert(QString::fromUtf8(repo->_worktree),
							QUuid(repo->_id));
		}
	}
	g_list_free(res);
}
