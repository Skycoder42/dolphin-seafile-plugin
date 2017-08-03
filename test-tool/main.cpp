#include <QCoreApplication>
#include <QDebug>
//extern "C" {
//#include <ccnet.h>
//#include <seafile/seafile-object.h>
//#include <seafile/seafile.h>
//}

//void listRepos(SearpcClient *client)
//{
//	GError *error = NULL;
//	//auto res = searpc_client_call__objlist(client, "seafile_get_repo_list", SEAFILE_TYPE_REPO, &error, 2, "int", 0, "int", 5);
//	auto res = seafile_get_repo_list(client, 0, -1, &error);
//	if(error)
//		qDebug() << error->message;
//	else {
//		for (auto l = res; l != NULL; l = l->next)
//		{
//			auto repo = SEAFILE_REPO(l->data);
//			qDebug() << repo->_id
//					 << repo->_name
//					 << repo->_worktree;
//		}
//	}
//	g_list_free(res);
//}

//void getSyncStatus(SearpcClient *client, const char *path)
//{
//	GError *error = NULL;
//	auto res = searpc_client_call__string(client, "seafile_get_path_sync_status", &error, 3,
//										  "string", "741d9814-968f-4b90-9139-575c1ad85cb0",
//										  "string", path,
//										  "int", 0);
//	if(error)
//		qDebug() << error->message;
//	else
//		qDebug() << path << res;
//	free(res);
//}

#include "seafstatus.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	SeafStatus status;
	qDebug() << status.syncStatus("/home/sky/Seafile/public/archive.kdbx");
	qDebug() << status.syncStatus("/home/sky/Seafile/public/.archive.kdbx.lock");
	qDebug() << status.syncStatus("/home/sky/Seafile/public/battery_measurements");
	qDebug() << status.syncStatus("/home/sky/.dir_colors");

//	auto pool = ccnet_client_pool_new("/home/sky/.ccnet/", "/home/sky/.ccnet/");
//	auto client = ccnet_create_pooled_rpc_client(pool, "06ef2337036f2a862e847967dc4f4c1106d27d9d", "seafile-rpcserver");

//	listRepos(client);
//	getSyncStatus(client, "archive.kdbx");
//	getSyncStatus(client, "/archive.kdbx");
//	getSyncStatus(client, "/.archive.kdbx.lock");

//	ccnet_rpc_client_free(client);
//	free(pool);

	return 0;
}
