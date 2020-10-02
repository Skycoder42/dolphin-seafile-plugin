#include "fileviewseafileplugin.h"

#include <KFileItem>
#include <QMenu>

#include <QDebug>

K_PLUGIN_FACTORY_DEFINITION_WITH_BASEFACTORY(FileViewSeafilePluginFactory, KPluginFactory, registerPlugin<FileViewSeafilePlugin>();)

FileViewSeafilePlugin::FileViewSeafilePlugin(QObject *parent, const QVariantList &args) :
	KVersionControlPlugin(parent),
	_seaf(new SeafStatus(this))
{
	Q_UNUSED(args)
}

QString FileViewSeafilePlugin::fileName() const
{
	return QStringLiteral(".seafile-data");
}

bool FileViewSeafilePlugin::beginRetrieval(const QString &directory)
{
	try {
		_seaf->engage();
		_seaf->reloadRepos();
		if(_seaf->hasRepo(directory))
			return true;
		else
			return false;
	} catch(QException &e) {
		emit errorMessage(QString::fromUtf8(e.what()));
		return false;
	}
}

void FileViewSeafilePlugin::endRetrieval()
{
	_seaf->disengage();
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
			return MissingVersion;
		case SeafStatus::Readonly:
			return RemovedVersion;
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
QList<QAction *> FileViewSeafilePlugin::versionControlActions(const KFileItemList &items) const
{
	Q_UNUSED(items)
	return {};
}

QList<QAction *> FileViewSeafilePlugin::outOfVersionControlActions(const KFileItemList &items) const
{
//	//test if this it's actually seafile
//	try {
//		_seaf->reloadRepos();
//		auto itemValid = false;
//		foreach(auto item, items) {
//			if(_seaf->hasRepo(item.localPath()))
//				itemValid = true;
//		}
//		if(!itemValid)
//			return {};
//	} catch(QException &e) {
//		qCritical() << e.what();
//		return {};
//	}

//	auto action = new QAction(tr("Seafile"), (QObject*)this);

//	auto menu = new QMenu();
//	action->setMenu(menu);
//	connect(action, &QAction::destroyed,
//			menu, &QMenu::deleteLater);

//	menu->addAction(QIcon::fromTheme(QStringLiteral("filename-ignore-amarok")),
//					tr("Ignore File(s)/Dir(s)"));
//	menu->addSeparator();
//	menu->addAction(QIcon::fromTheme(QStringLiteral("view-statistics")),
//					tr("Show Quota"));

//	return {
//		action
//	};
	Q_UNUSED(items)
	return {};
}
