#include "BTGame/BTGame.h"

/* constructor */
BTGame::BTGame()
{
	initBaseFunctions();
	initLogWindow();
	initSideBar();
	initCentralWindow();
	initMenu();
	readSettings();
}

BTGame::~BTGame()
{
	writeSettings();
}

void BTGame::initBaseFunctions()
{}

void BTGame::initLogWindow()
{
	logWindow = new LogWindow(this);
	addDockWidget(Qt::RightDockWidgetArea, logWindow);
	logWindow->hide();
}

void BTGame::initSideBar()
{
	sideBar = new SideBar;
	sideBar->resize(DEFAULT_SIDEBAR_WIDTH, height());
	addDockWidget(Qt::RightDockWidgetArea, sideBar);
}

void BTGame::initCentralWindow()
{
	map->stackUnder(sideBar);

	connect(map, &GraphicsMap::mechActionsNeeded, this, &BTGame::setActionsInSideBar);
	connect(map, &GraphicsMap::mechActionsNotNeeded, sideBar->getActionWindow(), &ActionWindow::clear);
	connect(map, &GraphicsMap::messageSent, this, &BTGame::messageFromMapToSideBar);
	connect(map, &GraphicsMap::extensiveInfoSent, this, &BTGame::onExtensiveInfo);

	connect(sideBar, &SideBar::endMoveButtonPressed, map, &GraphicsMap::onEndMove);
	connect(sideBar->getActionWindow(), &ActionWindow::actionActivated, map, &GraphicsMap::onChooseAction);

	connect(map, &GraphicsMap::gameEnded, this, &BTGame::onEndGame);

	connect(map, &GraphicsMap::mapCleared, sideBar->getActionWindow(), &ActionWindow::clear);
	connect(map, &GraphicsMap::mapCleared, sideBar, &SideBar::disable);
}

void BTGame::initMenu()
{
	menuTriggerLogWindow = new QAction(this);
	menuTriggerLogWindow->setEnabled(true);
	menuTriggerLogWindow->setText(BTech::Strings::ActionTriggerLogWindow);
	menuTriggerLogWindow->setShortcut(tr("Ctrl+L"));
	connect(menuTriggerLogWindow, &QAction::triggered, this, &BTGame::onTriggerLogWindow);
	viewMenu->addAction(menuTriggerLogWindow);

	gameMenu = menuBar()->addMenu(BTech::Strings::MenuGame);

	menuStartGameAction = new QAction(this);
	menuStartGameAction->setEnabled(false);
	menuStartGameAction->setText(BTech::Strings::ActionStartGame);
	menuStartGameAction->setShortcut(tr("Ctrl+F"));
	connect(menuStartGameAction, &QAction::triggered, this, &BTGame::onStartGameAction);
	gameMenu->addAction(menuStartGameAction);

	menuSetVersion = new QAction(this);
	menuSetVersion->setEnabled(false);
	menuSetVersion->setText(BTech::Strings::ActionSetVersion);
	menuSetVersion->setShortcut(tr("Ctrl+V"));
	connect(menuSetVersion, &QAction::triggered, this, &BTGame::onSetVersionAction);
	gameMenu->addAction(menuSetVersion);
}

void BTGame::readSettings()
{
	restoreGeometry(Settings::value("game/geometry").toByteArray());
}

void BTGame::writeSettings()
{
	Settings::setValue("game/geometry", saveGeometry());
	Settings::sync();
}

void BTGame::keyPressEvent(QKeyEvent *event)
{
	sideBar->keyPressEvent(event);
	return QWidget::keyPressEvent(event);
}

void BTGame::onTriggerLogWindow()
{
	logWindow->setVisible(!logWindow->isVisible());
}

void BTGame::onLoadMapAction()
{
	BTMapManager::onLoadMapAction();
	if (map->isLoaded()) {
		menuStartGameAction->setEnabled(true);
		menuSetVersion->setEnabled(true);
	}
}

void BTGame::onStartGameAction()
{
	sideBar->enable();
	menuStartGameAction->setEnabled(false);
	map->startGame();
}

void BTGame::onSetVersionAction()
{
	GameVersionChoiceDialog dialog(map->getAllowedVersionsRef());
	if (dialog.exec())
		Rules::setVersion(dialog.getVersion());
}

void BTGame::onEndGame()
{
	infoBar->hide();
	sideBar->disable();
}

void BTGame::setActionsInSideBar()
{
	sideBar->getActionWindow()->insertActions(map->getCurrentPhase(), map->getCurrentMech());
}

void BTGame::messageFromMapToSideBar()
{
	auto message = map->getMessage();
	sideBar->insertMessage(message.first, message.second);
}

void BTGame::onExtensiveInfo()
{
	logWindow->print(map->getExtInfo());
}

/**
 * \class GameVersionChoiceDialog
 */

GameVersionChoiceDialog::GameVersionChoiceDialog(const QList <BTech::GameVersion> &allowedVersions)
	: QDialog()
{
	setFixedSize(200, 300);

	QVBoxLayout *checkBoxLayout = new QVBoxLayout;
	checkBoxLayout->setAlignment(Qt::AlignTop);

	QVector <QCheckBox *> checkBox;
	group = new QButtonGroup;
	group->setExclusive(true);
	for (BTech::GameVersion version : allowedVersions) {
		checkBox << new QCheckBox(BTech::gameVersionStringChange[version]);
		group->addButton(checkBox.last());
		checkBoxLayout->addWidget(checkBox.last());
	}
	checkBox.first()->setChecked(true);

	QVBoxLayout *layout = new QVBoxLayout;

	layout->addItem(checkBoxLayout);

	confirmButton = new QPushButton(BTech::Strings::ButtonConfirm);
	connect(confirmButton, &QPushButton::pressed, this, &GameVersionChoiceDialog::accept);
	layout->addWidget(confirmButton);

	setLayout(layout);
}

BTech::GameVersion GameVersionChoiceDialog::getVersion() const
{
	return BTech::gameVersionStringChange[group->checkedButton()->text()];
}
