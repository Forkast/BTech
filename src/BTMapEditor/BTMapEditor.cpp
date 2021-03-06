#include "BTCommon/DataManager.h"
#include "BTCommon/Paths.h"
#include "BTCommon/Settings.h"
#include "BTMapEditor/BTMapEditor.h"

BTMapEditor::BTMapEditor()
{
	initInfoBar();
	initMap();
	initToolBar();
	initMenu();
	initSystem();
	readSettings();
}

BTMapEditor::~BTMapEditor()
{
	writeSettings();
}

void BTMapEditor::initInfoBar()
{
	infoBar->setDisplayState(InfoBar::DisplayState::HexWindowOnly);
}

void BTMapEditor::initMap()
{
	connect(map, static_cast<void (GraphicsMap::*)(Hex *)>(&GraphicsMap::hexClicked), this, &BTMapEditor::onHexClicked);
}

void BTMapEditor::initToolBar()
{
	toolBar = new ToolBar(map);

	connect(map, &GraphicsMap::mapHasBeenLoaded, toolBar, &ToolBar::onMapLoaded);

	connect(toolBar, &ToolBar::clickModeActivated, this, &BTMapEditor::onChooseClickMode);
	connect(toolBar, &ToolBar::playerChosen,       this, &BTMapEditor::onChoosePlayer);
	connect(toolBar, &ToolBar::unitChosen,         this, &BTMapEditor::onChooseUnit);
	connect(toolBar, &ToolBar::terrainChosen,      this, &BTMapEditor::onChooseTerrain);
	connect(toolBar, &ToolBar::playersInfoChanged, this, &BTMapEditor::updatePlayers);
	connect(toolBar, &ToolBar::hexesInfoChanged,   this, &BTMapEditor::updateHexes);

	addDockWidget(Qt::RightDockWidgetArea, toolBar);
}

void BTMapEditor::initMenu()
{
	editMenu = menuBar()->addMenu(BTech::Strings::MenuEdit);

	menuNewMapAction      = new QAction(this);
	menuSaveMapAction     = new QAction(this);
	menuSaveAsMapAction   = new QAction(this);
	menuEditWeaponsAction = new QAction(this);
	menuEditMechsAction   = new QAction(this);
	menuSaveDataAction    = new QAction(this);

	menuNewMapAction->setText(BTech::Strings::MenuActionNewMap);
	menuSaveMapAction->setText(BTech::Strings::MenuActionSaveMap);
	menuSaveAsMapAction->setText(BTech::Strings::MenuActionSaveAsMap);
	menuEditWeaponsAction->setText(BTech::Strings::MenuActionEditWeapons);
	menuEditMechsAction->setText(BTech::Strings::MenuActionEditMechs);
	menuSaveDataAction->setText(BTech::Strings::MenuActionSaveData);

	menuNewMapAction->setShortcut(QKeySequence::New);
	menuSaveMapAction->setShortcut(QKeySequence::Save);
	menuSaveAsMapAction->setShortcut(QKeySequence::SaveAs);

	connect(menuNewMapAction,      &QAction::triggered, this, &BTMapEditor::onNewMapAction);
	connect(menuSaveMapAction,     &QAction::triggered, this, &BTMapEditor::onSaveMapAction);
	connect(menuSaveAsMapAction,   &QAction::triggered, this, &BTMapEditor::onSaveAsMapAction);
	connect(menuEditWeaponsAction, &QAction::triggered, this, &BTMapEditor::onEditWeaponsAction);
	connect(menuEditMechsAction,   &QAction::triggered, this, &BTMapEditor::onEditMechsAction);
	connect(menuSaveDataAction,    &QAction::triggered, this, &BTMapEditor::onSaveData);

	fileMenu->addAction(menuNewMapAction);
	fileMenu->addAction(menuSaveMapAction);
	fileMenu->addAction(menuSaveAsMapAction);

	editMenu->addAction(menuEditWeaponsAction);
	editMenu->addAction(menuEditMechsAction);
	editMenu->addAction(menuSaveDataAction);

	sortMenu();
}

void BTMapEditor::sortMenu()
{
	fileMenu->clear();
	fileMenu->addAction(menuNewMapAction);
	fileMenu->addAction(menuLoadMapAction);
	fileMenu->addAction(menuSaveMapAction);
	fileMenu->addAction(menuSaveAsMapAction);
	fileMenu->addSeparator();
	fileMenu->addAction(menuQuitAction);
}

void BTMapEditor::initSystem()
{
	currentlyChosen = Chosen::Nothing;
	currentPlayer = nullptr;
	currentTerrain = BTech::Terrain::Clear;
	clickModeActive = false;
}

void BTMapEditor::readSettings()
{
	restoreGeometry(Settings::value("editor/geometry").toByteArray());
}

void BTMapEditor::writeSettings()
{
	Settings::setValue("editor/geometry", saveGeometry());
	Settings::sync();
}

bool BTMapEditor::saveMap(const QString &path)
{
	if (path.size() == 0)
		return false;
	QFile file(path);
	if (!file.open(QIODevice::WriteOnly))
		return false;
	QDataStream out(&file);
	map->setMapFileName(path);
	out << *map;
	file.close();

	return true;
}

void BTMapEditor::reloadData()
{
	toolBar->reloadData();
}

void BTMapEditor::onNewMapAction()
{
	NewMapDialog dialog(this);
	if (dialog.exec()) {
		map->createNewMap(dialog.getMapWidth(), dialog.getMapHeight());
		BTMapManager::startMapManagement();
	}
}

void BTMapEditor::onSaveMapAction()
{
	if (map->getMapFileName().isEmpty())
		return onSaveAsMapAction();
	saveMap(map->getMapFileName());
}

void BTMapEditor::onSaveAsMapAction()
{
	QString path = QFileDialog::getSaveFileName(this, BTech::Strings::DialogSaveMap, BTech::Paths::MAPS_PATH, BTech::Strings::DialogBTechMapFiles);
	if (saveMap(path))
		map->setMapFileName(path);
}

void BTMapEditor::onQuitAction()
{
	if (!WeaponModel::getInstance().isChanged() && !MechModel::getInstance().isChanged()) {
		BTMapManager::onQuitAction();
		return;
	}

	QMessageBox dialog(QMessageBox::NoIcon,
	                   BTech::Strings::DialogQuitWithoutSaving,
	                   BTech::Strings::QuitWithoutSavingWarning,
	                   QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel,
	                   this);

	dialog.exec();

	switch (dialog.buttonRole(dialog.clickedButton())) {
		case QMessageBox::YesRole: onSaveData();
		case QMessageBox::NoRole:  BTMapManager::onQuitAction();
		default:                   return;
	}
}

void BTMapEditor::onEditWeaponsAction()
{
	EditWeaponsDialog dialog(this);
	dialog.exec();
	reloadData();
}

void BTMapEditor::onEditMechsAction()
{
	EditMechsDialog dialog(this);
	dialog.exec();
	reloadData();
}

void BTMapEditor::onSaveData()
{
	if (!DataManager::saveToFile(BTech::Paths::DATA_PATH))
		qWarning() << "Save data failed!";
	else
		qDebug() << "Data saved.";
}

void BTMapEditor::onChoosePlayer(Player *player)
{
	currentPlayer = player;
}

void BTMapEditor::onChooseUnit(UID unitUid)
{
	currentlyChosen = Chosen::Mech;
	currentMech = unitUid;
}

void BTMapEditor::onChooseTerrain(BTech::Terrain terrain)
{
	currentlyChosen = Chosen::Terrain;
	currentTerrain = terrain;
}

void BTMapEditor::onChooseClickMode(bool is)
{
	clickModeActive = is;
	currentlyChosen = Chosen::Nothing;
	if (map->isLoaded())
		map->clearHexes();
}

void BTMapEditor::onHexClicked(Hex *hex)
{
	map->clearHexes();
	GraphicsFactory::get(hex)->setClicked(true);
	toolBar->onHexClicked(hex);
	switch (currentlyChosen) {
		case Chosen::Mech:
			map->addMechToHex(new MechEntity(currentMech), hex, currentPlayer);
			break; // responsibility for the new allocated MechEntity goes to GraphicsMap.
		case Chosen::Terrain:
			hex->setTerrain(currentTerrain);
			break;
		default:;
	}
}

void BTMapEditor::updatePlayers()
{
	map->updatePlayers();
}

void BTMapEditor::updateHexes()
{
	map->updateHexes();
}
