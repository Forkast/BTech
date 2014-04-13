#ifndef MAP_PROPERTIES_MANAGER_H
#define MAP_PROPERTIES_MANAGER_H

#include <QtWidgets>
#include "BTCommon/Rules.h"
#include "BTMapEditor/ManagersUtils.h"

/**
 * \class MapPropertiesManager
 */
class MapPropertiesManager : public QWidget
{
Q_OBJECT;

public:
	MapPropertiesManager(QString &mapDescription, QList <BTech::GameVersion> &allowedVersions);
	void refresh();

public slots:
	void onMapLoaded();

private:
	static const int DEFAULT_WIDTH = 200;

	QString &mapDescriptionRef;
	QList <BTech::GameVersion> &allowedVersions;

	QTextEdit *mapDescription;
	QPushButton *confirmSaveMapDescriptionButton;

	QPushButton *versionsButton;

	void initVersionsButton();

private slots:
	void saveMapDescription();
	void editVersions();
};

/**
 * \class GameVersionDialog
 */

class GameVersionDialog : public QDialog
{
Q_OBJECT;

public:
	GameVersionDialog(QWidget *parent = nullptr);

	QList <BTech::GameVersion> getAllowedVersions() const;
	void setChecked(BTech::GameVersion version, bool check);
	bool isChecked(BTech::GameVersion version) const;

private:
	static const int DEFAULT_WIDTH  = 200;
	static const int DEFAULT_HEIGHT = 300;

	QHash <BTech::GameVersion, QCheckBox *> checkBox;
	QPushButton *confirmButton;
	QLabel *errorLabel;

private slots:
	void tryConfirm();
};

#endif // MAP_PROPERTIES_MANAGER_H
