#include "fileviewseafileplugin.h"
#include <KPluginFactory>
#include <KFileItem>

K_PLUGIN_FACTORY(FileViewSeafilePluginFactory, registerPlugin<FileViewSeafilePlugin>();)

FileViewSeafilePlugin::FileViewSeafilePlugin(QObject *parent, const QVariantList &args) :
	KVersionControlPlugin(parent),
	_seaf(new SeafStatus(this))
{
	Q_UNUSED(args)
}

QString FileViewSeafilePlugin::fileName() const
{
	return QStringLiteral(".seafile-data");//TODO use home or whatever? to include ALL repos
}

bool FileViewSeafilePlugin::beginRetrieval(const QString &directory)
{
	Q_UNUSED(directory)
	try {
		_seaf->connect();
		return true;
	} catch(QException &e) {
		emit errorMessage(QString::fromUtf8(e.what()));
		return false;
	}
}

void FileViewSeafilePlugin::endRetrieval()
{
	_seaf->disconnect();
}

KVersionControlPlugin::ItemVersion FileViewSeafilePlugin::itemVersion(const KFileItem &item) const
{
	try {
		auto status = _seaf->syncStatus(item.localPath()); //TODO relative?
		switch (status) {
		case SeafStatus::None:
			return UnversionedVersion;
		case SeafStatus::Syncing:
			return UpdateRequiredVersion;
		case SeafStatus::Error:
			return ConflictingVersion; //TODO
		case SeafStatus::Ignored:
			return IgnoredVersion;
		case SeafStatus::Synced:
			return NormalVersion;
		case SeafStatus::Paused:
			return NormalVersion; //TODO
		case SeafStatus::Readonly:
			return LocallyModifiedUnstagedVersion; //TODO
		case SeafStatus::Locked:
			return LocallyModifiedVersion; //TODO
		case SeafStatus::LockedByMe:
			return LocallyModifiedVersion; //TODO
		case SeafStatus::Invalid:
			return MissingVersion;
		default:
			Q_UNREACHABLE();
			break;
		}
	} catch(QException &e) {
		//TODO emit errorMessage(QString::fromUtf8(e.what()));
		return UnversionedVersion;
	}
}

QList<QAction *> FileViewSeafilePlugin::actions(const KFileItemList &items) const
{
	Q_UNUSED(items);
	return {};
}
