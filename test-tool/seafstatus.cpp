#include "seafstatus.h"
#include <QDebug>
#include <QDir>
extern "C" {
#include <seafile/seafile-object.h>
#include <seafile/seafile.h>
}

SeafStatus::SeafStatus(QObject *parent) :
	QObject(parent),
	_pool(ccnet_client_pool_new(nullptr, DEFAULT_CONFIG_DIR)),
	_client(ccnet_create_pooled_rpc_client(_pool, nullptr, "seafile-rpcserver")),
	_repoIds()
{
	loadRepos();
}

SeafStatus::~SeafStatus()
{
	ccnet_rpc_client_free(_client);
	free(_pool);
}

QByteArray SeafStatus::syncStatus(const QString &path)
{
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
				return error->message;
			else
				return res;
			free(res);
		}
	}

	return "none";
}

void SeafStatus::loadRepos()
{
	GError *error = NULL;
	//auto res = searpc_client_call__objlist(client, "seafile_get_repo_list", SEAFILE_TYPE_REPO, &error, 2, "int", 0, "int", 5);
	auto res = seafile_get_repo_list(_client, 0, -1, &error);
	if(error)
		qCritical() << error->message;
	else {
		for (auto l = res; l != NULL; l = l->next) {
			auto repo = SEAFILE_REPO(l->data);
			_repoIds.insert(QString::fromUtf8(repo->_worktree),
							QUuid(repo->_id));
		}
	}
	g_list_free(res);
}
