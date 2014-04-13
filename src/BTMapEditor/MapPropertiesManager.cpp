#include "BTCommon/EnumHashFunctions.h"
#include "BTMapEditor/MapPropertiesManager.h"

/**
 * \class MapPropertiesManager
 */

MapPropertiesManager::MapPropertiesManager(QString &mapDescriptionRef, QList <BTech::GameVersion> &allowedVersions)
	: mapDescriptionRef(mapDescriptionRef), allowedVersions(allowedVersions)
{
	QLabel *mapDescriptionLabel = new QLabel(BTech::Strings::LabelMap);

	mapDescription = new QTextEdit;
	mapDescription->setEnabled(false);

	confirmSaveMapDescriptionButton = new QPushButton(BTech::Strings::ButtonSaveMapDescription);
	confirmSaveMapDescriptionButton->setFixedWidth(DEFAULT_WIDTH);
	connect(confirmSaveMapDescriptionButton, &QPushButton::pressed, this, &MapPropertiesManager::saveMapDescription);
	confirmSaveMapDescriptionButton->setEnabled(false);

	initVersionsButton();

	/* layout */

	QHBoxLayout *saveMapDescriptionButtonLayout = new QHBoxLayout;
	saveMapDescriptionButtonLayout->addSpacerItem(new QSpacerItem(BTech::ENORMOUS_SPACER_SIZE, BTech::SYMBOLIC_SPACER_SIZE));
	saveMapDescriptionButtonLayout->addWidget(confirmSaveMapDescriptionButton);

	QHBoxLayout *versionsButtonLayout = new QHBoxLayout;
	versionsButtonLayout->addSpacerItem(new QSpacerItem(BTech::ENORMOUS_SPACER_SIZE, BTech::SYMBOLIC_SPACER_SIZE));
	versionsButtonLayout->addWidget(versionsButton);


	QVBoxLayout *layout = new QVBoxLayout;
	layout->setAlignment(Qt::AlignTop);

	layout->addWidget(mapDescriptionLabel);
	layout->addWidget(mapDescription);
	layout->addItem(saveMapDescriptionButtonLayout);
	layout->addItem(versionsButtonLayout);

	setLayout(layout);
	refresh();
}

void MapPropertiesManager::refresh()
{
	mapDescription->setText(mapDescriptionRef);
}

void MapPropertiesManager::onMapLoaded()
{
	mapDescription->setEnabled(true);
	confirmSaveMapDescriptionButton->setEnabled(true);
	versionsButton->setEnabled(true);
	refresh();
}

void MapPropertiesManager::initVersionsButton()
{
	versionsButton = new QPushButton(BTech::Strings::ButtonVersions);
	versionsButton->setFixedWidth(BTech::NAME_LINE_WIDTH);
	connect(versionsButton, &QPushButton::pressed, this, &MapPropertiesManager::editVersions);
	versionsButton->setEnabled(false);
}

void MapPropertiesManager::saveMapDescription()
{
	mapDescriptionRef = mapDescription->document()->toPlainText();
	refresh();
}

void MapPropertiesManager::editVersions()
{
	GameVersionDialog dialog;
	for (BTech::GameVersion gameVersion : BTech::gameVersions)
		dialog.setChecked(gameVersion, allowedVersions.contains(gameVersion));
	if (dialog.exec())
		allowedVersions = QList <BTech::GameVersion> (dialog.getAllowedVersions());
}

/**
 * \class GameVersionDialog
 */

GameVersionDialog::GameVersionDialog(QWidget *parent)
	: QDialog(parent)
{
	setFixedSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);

	QVBoxLayout *checkBoxLayout = new QVBoxLayout;
	checkBoxLayout->setAlignment(Qt::AlignTop);
	for (BTech::GameVersion gameVersion : BTech::gameVersions) {
		QCheckBox *newCheckBox = new QCheckBox(BTech::gameVersionStringChange[gameVersion]);
		newCheckBox->setChecked(true);
		checkBoxLayout->addWidget(newCheckBox);
		checkBox[gameVersion] = newCheckBox;
	}

	QVBoxLayout *layout = new QVBoxLayout;

	layout->addItem(checkBoxLayout);

	errorLabel = new QLabel(BTech::Strings::GameVersionDialogWarning);
	errorLabel->setWordWrap(true);
	errorLabel->setVisible(false);
	layout->addWidget(errorLabel);

	confirmButton = new QPushButton(BTech::Strings::ButtonConfirm);
	connect(confirmButton, &QPushButton::pressed, this, &GameVersionDialog::tryConfirm);
	layout->addWidget(confirmButton);

	setLayout(layout);
}

QList <BTech::GameVersion> GameVersionDialog::getAllowedVersions() const
{
	QList <BTech::GameVersion> list;
	for (BTech::GameVersion version : BTech::gameVersions)
		if (checkBox[version]->isChecked())
			list.append(version);
	return list;
}

void GameVersionDialog::setChecked(BTech::GameVersion version, bool check)
{
	checkBox[version]->setChecked(check);
}

bool GameVersionDialog::isChecked(BTech::GameVersion version) const
{
	return checkBox[version]->isChecked();
}

void GameVersionDialog::tryConfirm()
{
	bool checked = false;
	for (BTech::GameVersion version : BTech::gameVersions)
		checked |= checkBox[version]->isChecked();
	if (checked)
		QDialog::accept();
	else
		errorLabel->setVisible(true);
}
