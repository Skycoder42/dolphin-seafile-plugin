#ifndef FILEVIEWSEAFILEPLUGIN_H
#define FILEVIEWSEAFILEPLUGIN_H

#include <KVersionControlPlugin>

class FileViewSeafilePlugin : public KVersionControlPlugin
{
	Q_OBJECT

public:
	FileViewSeafilePlugin(QObject* parent, const QVariantList& args);

	// KVersionControlPlugin interface
public:
	QString fileName() const override;
	bool beginRetrieval(const QString &directory) override;
	void endRetrieval() override;
	ItemVersion itemVersion(const KFileItem &item) const override;
	QList<QAction *> actions(const KFileItemList &items) const override;
};

#endif // FILEVIEWSEAFILEPLUGIN_H
