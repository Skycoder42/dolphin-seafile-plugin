#include "seafstatus.h"
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QTextStream>

extern "C" {
#include <searpc-named-pipe-transport.h>
#include <searpc.h>
#include <seafile/seafile-object.h>
#include <seafile/seafile.h>
}

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

	GError *error = nullptr;
	auto res = seafile_get_repo_list(_client, 0, -1, &error);
	if(error)
		throw SeafException(error);
	else {
		for (auto l = res; l != nullptr; l = l->next) {
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

	auto repo = repoPath(path);
	//TODO handle repository folder -> should show a global sync state of repo
	if(!repo.isNull()) {
		QDir repoDir(repo);
		auto id = _repoIds.value(repo);
		auto idString = id.toByteArray();
		idString = idString.mid(1, idString.size() - 2);

		GError *error = nullptr;
		auto res = searpc_client_call__string(_client, "seafile_get_path_sync_status", &error, 3,
											  "string", idString.constData(),
											  "string", repoDir.relativeFilePath(path).toUtf8().constData(),
											  "int", QFileInfo(path).isDir());
		if(error)
			throw SeafException(error);
		else {
			auto status = mapStatus(res);
			free(res);
			return status;
		}
	}

	return None;
}

void SeafStatus::ensureConnected()
{
	if(!_client) {
		// find the socket
		auto seafDir = readSeafileIni();
		if(seafDir.isNull())
			seafDir = QDir::home().filePath(QStringLiteral("Seafile/.seafile-data"));
		auto path = QDir{seafDir}.absoluteFilePath(QStringLiteral("seafile.sock"));

		// create the client
		auto pipe_client = searpc_create_named_pipe_client(qUtf8Printable(path));
		int ret = searpc_named_pipe_client_connect(pipe_client);
		_client = searpc_client_with_named_pipe_transport(pipe_client, "seafile-rpcserver");
		if (ret < 0) {
		   searpc_free_client_with_pipe_transport(_client);
		   _client = nullptr;
		   throw SeafException(tr("Unable to create piped searpc client for seafile-rpcserver"));
		}

		reloadRepos();
	}

	_conTimer->start();
}

void SeafStatus::freeConnection()
{
	if(_client){
		searpc_free_client_with_pipe_transport(_client);
		_client = nullptr;
	}
}

QString SeafStatus::repoPath(const QString &path)
{
	QFileInfo info(path);
	auto fullPath = QDir::cleanPath(info.absoluteFilePath());
	for(auto it = _repoIds.constBegin(); it != _repoIds.constEnd(); it++) {
		if(fullPath.startsWith(it.key()))
			return it.key();
	}

	return QString();
}

SeafStatus::SyncStatus SeafStatus::mapStatus(const QByteArray &text)
{
	static const QHash<QByteArray, SeafStatus::SyncStatus> PathStatus {
		{"none", None},
		{"syncing", Syncing},
		{"error", Error},
		{"ignored", Ignored},
		{"synced", Synced},
		{"paused", Paused},
		{"readonly", Readonly},
		{"locked", Locked},
		{"locked_by_me", LockedByMe}
	};
	return PathStatus.value(text, Invalid);
}

QString SeafStatus::readSeafileIni()
{
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
	auto env = qEnvironmentVariable("SEAFILE_DATA_DIR");
	if(!env.isNull())
		return env;
	auto ccnetDir = qEnvironmentVariable("CCNET_CONF_DIR");
#else
	auto env = qgetenv("SEAFILE_DATA_DIR");
	if(!env.isNull())
		return QString::fromUtf8(env);
	auto ccnetDir = QString::fromUtf8(qgetenv("CCNET_CONF_DIR"));
#endif
	if(ccnetDir.isNull())
		ccnetDir = QDir::home().filePath(QStringLiteral(".ccnet"));

	QFile seafile_ini(QDir{ccnetDir}.filePath(QStringLiteral("seafile.ini")));
	if (!seafile_ini.exists())
		return {};

	if(!seafile_ini.open(QIODevice::ReadOnly | QIODevice::Text))
		throw SeafException(tr("%1: %2").arg(seafile_ini.fileName(), seafile_ini.errorString()));

	QTextStream input(&seafile_ini);
	input.setCodec("UTF-8");
	if (input.atEnd())
		return {};

	return input.readLine();
}



SeafException::SeafException(GError *error) :
	SeafException(SeafStatus::tr("Seafile Extension: %1").arg(QString::fromUtf8(error->message)).toUtf8())
{
	g_error_free(error);
}

SeafException::SeafException(const QString &message) :
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
	_message(std::move(message))
{}
