#include "BTMapEditor/PlayersManager.h"

PlayersManager::PlayersManager(QVector <Player *> &players, QList <BTech::GameVersion> &allowedVersions)
	: players(players), allowedVersions(allowedVersions)
{
	QLabel *playersLabel = new QLabel(BTech::Strings::LabelPlayers);

	playersComboBox = new QComboBox;
	connect(playersComboBox, static_cast<void (QComboBox::*)(int)>(&QComboBox::activated), this, &PlayersManager::onPlayerChosen);

	addPlayerButton = new QPushButton(BTech::Strings::ButtonAddNewPlayer);
	connect(addPlayerButton, &QPushButton::pressed, this, &PlayersManager::addNewPlayer);
	addPlayerButton->setEnabled(false);
	removePlayerButton = new QPushButton(BTech::Strings::ButtonRemovePlayer);
	connect(removePlayerButton, &QPushButton::pressed, this, &PlayersManager::removePlayer);

	QLabel *playerNameLabel = new QLabel(BTech::Strings::LabelName);

	playerName = new QLineEdit;
	playerDescription = new QTextEdit;
	playerNameLabel->setBuddy(playerName);

	confirmSavePlayerButton = new QPushButton(BTech::Strings::ButtonSavePlayerDescription);
	confirmSavePlayerButton->setFixedWidth(200);
	connect(confirmSavePlayerButton, &QPushButton::pressed, this, &PlayersManager::savePlayer);
	confirmSavePlayerButton->setEnabled(false);

	/* layout */

	QHBoxLayout *playerButtonsLayout = new QHBoxLayout;
	playerButtonsLayout->addWidget(addPlayerButton);
	playerButtonsLayout->addWidget(removePlayerButton);

	QHBoxLayout *playerNameLayout = new QHBoxLayout;
	playerNameLayout->addWidget(playerNameLabel);
	playerNameLayout->addSpacerItem(new QSpacerItem(BTech::SMALL_SPACER_SIZE, BTech::SYMBOLIC_SPACER_SIZE));
	playerNameLayout->addWidget(playerName);

	QHBoxLayout *savePlayerButtonLayout = new QHBoxLayout;
	savePlayerButtonLayout->addSpacerItem(new QSpacerItem(BTech::ENORMOUS_SPACER_SIZE, BTech::SYMBOLIC_SPACER_SIZE));
	savePlayerButtonLayout->addWidget(confirmSavePlayerButton);

	QVBoxLayout *layout = new QVBoxLayout;
	layout->setAlignment(Qt::AlignTop);

	layout->addWidget(playersLabel);
	layout->addWidget(playersComboBox);
	layout->addItem(playerButtonsLayout);
	layout->addItem(playerNameLayout);
	layout->addWidget(playerDescription);
	layout->addItem(savePlayerButtonLayout);

	setLayout(layout);
	refresh();
}

void PlayersManager::setPlayers(QVector <Player *> &players)
{
	this->players = players;
	refresh();
}

void PlayersManager::refresh()
{
	playersComboBox->clear();
	for (Player *player : players)
		playersComboBox->addItem(player->getName());
	if (getCurrentPlayer() != nullptr) {
		playerName->setText(getCurrentPlayer()->getName());
		playerDescription->setText(getCurrentPlayer()->getDescription());
	}

	bool enabled = !players.empty();
	playersComboBox->setEnabled(enabled);
	removePlayerButton->setEnabled(enabled);
	if (!enabled) {
		playerName->clear();
		playerDescription->clear();
	}
	playerName->setEnabled(enabled);
	playerDescription->setEnabled(enabled);
	confirmSavePlayerButton->setEnabled(enabled);
}

void PlayersManager::onMapLoaded()
{
	addPlayerButton->setEnabled(true);
	confirmSavePlayerButton->setEnabled(true);
	refresh();
}

Player * PlayersManager::getPlayer(const QString &name) const
{
	for (Player *player : players)
		if (player->getName() == name)
			return player;
	return nullptr;
}

Player * PlayersManager::getCurrentPlayer() const
{
	return getPlayer(playersComboBox->currentText());
}

void PlayersManager::onPlayerChosen()
{
	playerName->setText(getCurrentPlayer()->getName());
	playerDescription->setText(getCurrentPlayer()->getDescription());
	emit playerChosen(getCurrentPlayer());
}

void PlayersManager::addNewPlayer()
{
	if (players.size() == MAX_PLAYERS_SIZE)
		return;

	QList <QString> existingNames;
	for (Player *player : players)
		existingNames.append(player->getName());
	QString newName = BTech::General::indexString(BTech::Strings::UnnamedPlayer, existingNames);

	Player *player = new Player;
	player->setName(newName);

	players.append(player);
	refresh();
	emit playerAdded();
}

void PlayersManager::removePlayer()
{
	emit playerNeedsRemoving(getCurrentPlayer());
	emit playerRemoved();
}

void PlayersManager::savePlayer()
{
	getCurrentPlayer()->setDescription(playerDescription->document()->toPlainText());
	getCurrentPlayer()->setName(playerName->text());
	refresh();
	emit playerInfoChanged();
}
