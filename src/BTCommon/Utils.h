#ifndef UTILS_H
#define UTILS_H

#include "BTCommon/BiHash.h"
#include "BTCommon/Colors.h"
#include "BTCommon/CommonStrings.h"
#include "BTCommon/EnumSerialization.h"

#define repeat_twice for (int repeat_index_ = 0; repeat_index_ < 2; ++repeat_index_)

namespace BTech {

	/**
	* \enum GamePhase
	* Contains all phases that take place in any playable version of the game.
	*/
	enum class GamePhase : qint8 {
		None           = -1,
		Initiative     =  0,
		Movement       =  1,
		Reaction       =  2,
		WeaponAttack   =  3,
		PhysicalAttack =  4,
		Combat         =  5,
		Heat           =  6,
		End            =  7
	};

	static const QList <GamePhase> phases {
		GamePhase::Initiative,
		GamePhase::Movement,
		GamePhase::Reaction,
		GamePhase::WeaponAttack,
		GamePhase::PhysicalAttack,
		GamePhase::Combat,
		GamePhase::Heat,
		GamePhase::End,
	};

	extern const BiHash <GamePhase, QString> phaseStringChange;

	/**
	 * \enum GameVersion
	 */
	enum class GameVersion : quint8 {
		BasicBattleDroids,
		AdvancedBattleDroids,
	};

	static const QList <GameVersion> gameVersions {
		GameVersion::BasicBattleDroids,
		GameVersion::AdvancedBattleDroids
	};

	extern const BiHash <GameVersion, QString> gameVersionStringChange;

	extern const QHash <GameVersion, QList <GamePhase> > versionToPhaseList;

	QDataStream & operator << (QDataStream &out, const GameVersion &rules);
	QDataStream & operator >> (QDataStream &in, GameVersion &rules);

	/**
	 * \enum EntityType
	 */

	enum class EntityType : quint8 {
		Mech
	};

	extern const BiHash <BTech::EntityType, QString> entityStringChange;

	//TODO CFiend udokumentować, czemu to jest jedno z niewielu miejsc,
	//gdzie wartość enumów ma rzeczywiste znaczenie dla działania programu

	/**
	 * \enum MechPartType
	 */
	enum class MechPartType : quint8 {
		Head  = 0x00,
		Torso = 0x01,
		Arm   = 0x02,
		Leg   = 0x03,
	};

	extern const BiHash <MechPartType, QString> mechPartTypeStringChange;

	QDataStream & operator << (QDataStream &out, const MechPartType &mechPartType);
	QDataStream & operator >> (QDataStream &in, MechPartType &mechPartType);

	/**
	 * \enum MechPartSide
	 */
	enum class MechPartSide : quint8 {
		General = 0x10,
		Center  = 0x20,
		Left    = 0x30,
		Right   = 0x40,
		Front   = 0x50,
		Rear    = 0x60,
	};

	extern const BiHash <MechPartSide, QString> mechPartSideStringChange;

	QDataStream & operator << (QDataStream &out, const MechPartSide &mechPartSide);
	QDataStream & operator >> (QDataStream &in, MechPartSide &mechPartSide);

	/**
	 * \enum WeaponType
	 */
	enum class WeaponType : quint8 {
		Energy    = 0,
		Ballistic = 1,
		Missile   = 2,
	};

	extern const BiHash <WeaponType, QString> weaponTypeStringChange;

	QDataStream & operator << (QDataStream &out, const WeaponType &weaponType);
	QDataStream & operator >> (QDataStream &in, WeaponType &weaponType);

	/**
	 * \typedef DiceRoll
	 */
	typedef int DiceRoll;
	DiceRoll dThrow();
	DiceRoll d2Throw();
	bool checkRoll(int value);

	static const DiceRoll MIN_DIE_ROLL = 1;
	static const DiceRoll MAX_DIE_ROLL = 6;
	static const DiceRoll MIN_TWO_DICE_ROLL = 2;
	static const DiceRoll MAX_TWO_DICE_ROLL = 12;

	/**
	 * \enum Range
	 */
	enum class Range : quint8 {
		Contact    = 0,
		Short      = 1,
		Medium     = 2,
		Long       = 3,
		OutOfRange = 4
	};

	static const std::array <Range, 5> ranges {
		Range::Contact,
		Range::Short,
		Range::Medium,
		Range::Long,
		Range::OutOfRange,
	};
	static const std::array <Range, 4> attackRanges {
		Range::Contact,
		Range::Short,
		Range::Medium,
		Range::Long,
	};
	static const std::array <Range, 3> shootRanges {
		Range::Short,
		Range::Medium,
		Range::Long,
	};

	extern const BiHash <Range, QString> rangeStringChange;

	static const int INF_RANGE_MODIFIER = 100;
	static const int MIN_RANGE_MODIFIER = -10;
	static const int MAX_RANGE_MODIFIER = 10;

	extern const QHash <Range, int> MAX_RANGE;

	QDataStream & operator << (QDataStream &out, const Range &range);
	QDataStream & operator >> (QDataStream &in, Range &range);

	/**
	 * \enum EffectType
	 */
	enum class EffectType : quint16 {
		None           = 0x000,

		Destroyed      = 0x001,
		ShutDown       = 0x002,
		Immobilised    = 0x003,
		Slowed         = 0x004,
		CannotRun      = 0x005,
		CannotShoot    = 0x006,
		CannotAttack   = 0x007,
		AimingBothered = 0x008,

		Walked         = 0x101,
		Run            = 0x102,
		Jumped         = 0x103,
	};

	extern const BiHash <EffectType, QString> effectTypeStringChange;

	QDataStream & operator << (QDataStream &out, const EffectType &effect);
	QDataStream & operator >> (QDataStream &in, EffectType &effect);

	/**
	 * \enum EffectSource
	 */
	enum class EffectSource : quint16 {
		NoSource = 0,
		Attack   = 1,
		Heat     = 2,
		Movement = 3,
	};

	QDataStream & operator << (QDataStream &out, const EffectSource &source);
	QDataStream & operator >> (QDataStream &in, EffectSource &source);

	/**
	 * \enum MovementAction
	 */
	enum class MovementAction : quint8 {
		Idle,
		Walk,
		Run,
		Jump,
		TurnRight,
		TurnLeft,
	};

	const QList <MovementAction> movementActions = {
		MovementAction::Idle,
		MovementAction::Walk,
		MovementAction::Run,
		MovementAction::Jump,
		MovementAction::TurnRight, // only for rotate-only movement
		MovementAction::TurnLeft,
	};

	QDataStream & operator << (QDataStream &out, const MovementAction &action);
	QDataStream & operator >> (QDataStream &in, MovementAction &action);

	/**
	 * \enum CombatAction
	 */
	enum class CombatAction : quint8 {
		Idle,
		SimpleAttack,
		WeaponAttack,
		Punch,
		Kick,
		Push,
		Charge
	};

	const QList <CombatAction> combatActions = {
		CombatAction::Idle,
		CombatAction::SimpleAttack,
		CombatAction::WeaponAttack,
		CombatAction::Punch,
		CombatAction::Kick,
		CombatAction::Push,
		CombatAction::Charge,
	};

	QDataStream & operator << (QDataStream &out, const CombatAction &action);
	QDataStream & operator >> (QDataStream &in, CombatAction &action);

	/**
	 * \function random
	 */

	int randomInt();
	int randomInt(int modulo);

	/**
	* \namespace General
	* This namespace contains functions too general to be put in GameSystem. They do not use game-specific classes.
	*/
	namespace General {
		QString indexString(const QString &string, const QList <QString> &existing);

		// world timer
		void startTime();
		unsigned long long getTime();
	}

	/**
	* \namespace Math
	* Contains some useful constants and functions that Qt does not provide.
	*/
	namespace Math {
		const int FULL_ANGLE = 360;
		const int STRAIGHT_ANGLE = FULL_ANGLE / 2;
		const int HEX_ANGLE = FULL_ANGLE / 6;

		int lineAngle(const QPoint &src, const QPoint &dest);
		int lineAngle(const QPoint &src, const QPointF &dest);
		int lineAngle(const QPointF &src, const QPoint &dest);
	}

	/**
	* \class Matrix
	* Enables the simplest matrix operations for Z(2, 2)
	*/
	class Matrix {
	public:
		int x1, y1, x2, y2;

		Matrix(int x1 = 0, int y1 = 0,
		       int x2 = 0, int y2 = 0);
		Matrix & operator = (Matrix arg);
		int det() const;
	};
}

#endif // UTILS_H
