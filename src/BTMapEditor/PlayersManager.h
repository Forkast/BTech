#ifndef PLAYERS_MANAGER_H
#define PLAYERS_MANAGER_H

#include <QtWidgets>
#include "BTCommon/Rules.h"
#include "BTMapEditor/ManagersUtils.h"

/**
 * \class PlayersManager
 */
class PlayersManager : public QWidget
{
Q_OBJECT;

public:
	PlayersManager(QVector <Player *> &players, QList <BTech::GameVersion> &allowedVersions);

	void setPlayers(QVector <Player *> &players);
	void refresh();

public slots:
	void onMapLoaded();

signals:
	void playerChosen(Player *player);
	void playerAdded();
	void playerInfoChanged();
	void playerNeedsRemoving(Player *player);
	void playerRemoved();

private:
	static const int MAX_PLAYERS_SIZE = 8;

	static const int DEFAULT_WIDTH = 200;

	QVector <Player *> &players;
	QList <BTech::GameVersion> &allowedVersions;

	QComboBox *playersComboBox;
	QPushButton *addPlayerButton;
	QPushButton *removePlayerButton;
	QLineEdit *playerName;
	QTextEdit *playerDescription;
	QPushButton *confirmSavePlayerButton;

	Player * getPlayer(const QString &name) const;
	Player * getCurrentPlayer() const;

private slots:
	void onPlayerChosen();
	void addNewPlayer();
	void removePlayer();
	void savePlayer();
};


#endif