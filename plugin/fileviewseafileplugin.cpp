#include "fileviewseafileplugin.h"

#include <KPluginFactory>
#include <KFileItem>

#include <QDebug>

K_PLUGIN_FACTORY(FileViewSeafilePluginFactory, registerPlugin<FileViewSeafilePlugin>();)

FileViewSeafilePlugin::FileViewSeafilePlugin(QObject *parent, const QVariantList &args) :
	KVersionControlPlugin(parent),
	_seaf(new SeafStatus(this))
{
	Q_UNUSED(args)
}

QString FileViewSeafilePlugin::fileName() const
{
	return QStringLiteral(".seafile-data");//TODO works, but not for repos outside of the "seafile dir"
}

bool FileViewSeafilePlugin::beginRetrieval(const QString &directory)
{
	Q_UNUSED(directory)
	try {
		_seaf->connectCcnet();
		return true;
	} catch(QException &e) {
		emit errorMessage(QString::fromUtf8(e.what()));
		return false;
	}
}

void FileViewSeafilePlugin::endRetrieval()
{
	_seaf->disconnectCcnet();
}

KVersionControlPlugin::ItemVersion FileViewSeafilePlugin::itemVersion(const KFileItem &item) const
{
	try {
		auto status = _seaf->syncStatus(item.localPath());

		switch (status) {
		case SeafStatus::None:
			return UnversionedVersion;
		case SeafStatus::Syncing:
			return UpdateRequiredVersion;
		case SeafStatus::Error:
			return ConflictingVersion;
		case SeafStatus::Ignored:
			return IgnoredVersion;
		case SeafStatus::Synced:
			return NormalVersion;
		case SeafStatus::Paused:
			return MissingVersion; //TODO other ItemVersion?
		case SeafStatus::Readonly:
			return RemovedVersion; //TODO other ItemVersion?
		case SeafStatus::Locked:
			return LocallyModifiedUnstagedVersion;
		case SeafStatus::LockedByMe:
			return LocallyModifiedVersion;
		case SeafStatus::Invalid:
			return MissingVersion;
		default:
			Q_UNREACHABLE();
			return UnversionedVersion;
		}
	} catch(QException &e) {
		qCritical() << e.what();
		return UnversionedVersion;
	}
}

QList<QAction *> FileViewSeafilePlugin::actions(const KFileItemList &items) const
{
	Q_UNUSED(items);
	return {
		new QAction("Test Action", (QObject*)this)
	};
}

#include "fileviewseafileplugin.moc"
