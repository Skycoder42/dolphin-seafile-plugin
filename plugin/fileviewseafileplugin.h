#ifndef FILEVIEWSEAFILEPLUGIN_H
#define FILEVIEWSEAFILEPLUGIN_H

#include <Dolphin/KVersionControlPlugin>
#include <KPluginFactory>

#include "seafstatus.h"

class FileViewSeafilePlugin : public KVersionControlPlugin
{
	Q_OBJECT

public:
	FileViewSeafilePlugin(QObject* parent, const QVariantList &args);

	QString fileName() const override;
	bool beginRetrieval(const QString &directory) override;
	void endRetrieval() override;
	ItemVersion itemVersion(const KFileItem &item) const override;
	QList<QAction *> actions(const KFileItemList &items) const override;

private:
	SeafStatus *_seaf;
};

K_PLUGIN_FACTORY_DECLARATION_WITH_BASEFACTORY(FileViewSeafilePluginFactory, KPluginFactory)

#endif // FILEVIEWSEAFILEPLUGIN_H
