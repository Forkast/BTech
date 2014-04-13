#include "BTMapEditor/ToolBar.h"

/**
 * \class ToolBar
 */

ToolBar::ToolBar(Map *map)
{
	this->map = map;
	initManagers(map->getPlayers(), map->getHexes(), map->getDescriptionReference(), map->getAllowedVersionsRef());
	initTabs();
	initWindow();
	initWidget();
	initLayout();
}

void ToolBar::setPlayers(QVector <Player *> &players)
{
	playersManager->setPlayers(players);
}

void ToolBar::setHexes(QVector <Hex *> &hexes)
{}

void ToolBar::reloadData()
{
	// TODO called by the main window, when data has changed - meaning reload data about mechs and weapons in the graphical interface
}

void ToolBar::refresh()
{
	mapPropertiesManager->refresh();
	playersManager->refresh();
	unitsManager->refresh();
}

void ToolBar::onMapLoaded()
{
	mapPropertiesManager->onMapLoaded();
	playersManager->onMapLoaded();
	unitsManager->onMapLoaded();
}

void ToolBar::onHexClicked(Hex *hex)
{
	clickModeManager->onHexClicked(hex);
}

void ToolBar::initTabs()
{
	tabs = new QTabWidget;
	tabs->addTab(mapPropertiesManager, BTech::Strings::LabelMapProperties);
	tabs->addTab(playersManager,       BTech::Strings::LabelPlayers);
	tabs->addTab(unitsManager,         BTech::Strings::LabelUnits);
	tabs->addTab(terrainManager,       BTech::Strings::LabelTerrain);
	tabs->addTab(clickModeManager,     BTech::Strings::LabelClickMode);

	connect(tabs, &QTabWidget::currentChanged, this, &ToolBar::onTabChosen);
}

void ToolBar::initManagers(QVector <Player *> &players, QVector <Hex *> &hexes, QString &mapDescriptionRef, QList <BTech::GameVersion> &allowedVersions)
{
	playersManager = new PlayersManager(players, allowedVersions);
	connect(playersManager, &PlayersManager::playerChosen,        this, &ToolBar::playerChosen);
	connect(playersManager, &PlayersManager::playerNeedsRemoving, this, &ToolBar::removePlayer);
	connect(playersManager, &PlayersManager::playerAdded,         this, &ToolBar::refresh);
	connect(playersManager, &PlayersManager::playerRemoved,       this, &ToolBar::refresh);
	connect(playersManager, &PlayersManager::playerAdded,         this, &ToolBar::playersInfoChanged);
	connect(playersManager, &PlayersManager::playerRemoved,       this, &ToolBar::playersInfoChanged);
	connect(playersManager, &PlayersManager::playerInfoChanged,   this, &ToolBar::playersInfoChanged);

	unitsManager = new UnitsManager(players);
	connect(unitsManager,         &UnitsManager::playerChosen,              this,         &ToolBar::playerChosen);
	connect(unitsManager,         &UnitsManager::unitChosen,                this,         &ToolBar::unitChosen);
	connect(playersManager, &PlayersManager::playerInfoChanged, unitsManager, &UnitsManager::refresh);

	terrainManager = new TerrainManager;
	connect(terrainManager, &TerrainManager::terrainChosen, this, &ToolBar::terrainChosen);

	clickModeManager = new ClickModeManager;
	connect(clickModeManager, &ClickModeManager::hexInfoChanged,    this, &ToolBar::hexesInfoChanged);
	connect(clickModeManager, &ClickModeManager::mechNeedsRemoving, this, &ToolBar::removeMech);

	mapPropertiesManager = new MapPropertiesManager(mapDescriptionRef, allowedVersions);
}

void ToolBar::initWindow()
{
	setMouseTracking(true);
	setFeatures(QDockWidget::DockWidgetMovable | QDockWidget::DockWidgetFloatable);
	setBaseSize(DEFAULT_WIDTH, DEFAULT_HEIGHT);
	setContentsMargins(0, 0, 0, 0);
}

void ToolBar::initWidget()
{
	setWidget(new QWidget(this));
	widget()->setMouseTracking(true);
}

void ToolBar::initLayout()
{
	QVBoxLayout *layout = new QVBoxLayout;

	layout->addWidget(tabs);

	widget()->setContentsMargins(0, 0, 0, 0);
	widget()->setLayout(layout);
}

void ToolBar::onTabChosen(int number)
{
	emit clickModeActivated(number == CLICK_MODE_OVERLAP_INDEX);
}

void ToolBar::removePlayer(Player *player)
{
	map->removePlayer(player);
}

void ToolBar::removeMech(MechEntity *mech)
{
	for (Player *player : map->getPlayers())
		for (MechEntity *cur : player->getMechs())
			if (mech == cur)
				player->removeMech(mech);
	emit hexesInfoChanged();
}
