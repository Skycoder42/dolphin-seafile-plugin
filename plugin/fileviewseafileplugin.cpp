#include "fileviewseafileplugin.h"
#include <KPluginFactory>

K_PLUGIN_FACTORY(FileViewSeafilePluginFactory, registerPlugin<FileViewSeafilePlugin>();)

FileViewSeafilePlugin::FileViewSeafilePlugin(QObject *parent, const QVariantList &args) :
	KVersionControlPlugin(parent)
{}

QString FileViewSeafilePlugin::fileName() const
{
	return QStringLiteral(".seafile-data");//TODO use home or whatever? to include ALL repos
}

bool FileViewSeafilePlugin::beginRetrieval(const QString &directory)
{
	//TODO connect to ccnet and load repositories
	return true;
}

void FileViewSeafilePlugin::endRetrieval()
{
	//TODO disconnect
}

KVersionControlPlugin::ItemVersion FileViewSeafilePlugin::itemVersion(const KFileItem &item) const
{
	//TODO load sync state for file
	return NormalVersion;
}

QList<QAction *> FileViewSeafilePlugin::actions(const KFileItemList &items) const
{
	return {};
}
