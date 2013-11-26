//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef CONSTANTS_H
#define CONSTANTS_H

//|||||||||||||||||||||||||||||||||||||||||||||||

enum QueryFlags
{
	OGRE_HEAD_MASK	= 1<<0,
	CUBE_MASK		= 1<<1,
	NINJA_MASK		= 1<<2,
	ROBOT_MASK		= 1<<3
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#define NUM_ANIMS 13           // number of animations the character has
#define CHAR_HEIGHT 5          // height of character's center of mass above ground
#define CAM_HEIGHT 2           // height of camera above character's center of mass
#define RUN_SPEED 0.1f           // character running speed in units per second
#define TURN_SPEED 50.0f      // character turning in degrees per second
#define ANIM_FADE_SPEED 7.5f   // animation crossfade speed in % of full weight per second
#define JUMP_ACCEL 30.0f       // character jump acceleration in upward units per squared second
#define GRAVITY 90.0f          // gravity in downward units per squared second

//////////////////////////////////////////////////////////////////////////

enum EnumWeaponType
{
	en_Weapon1 = 0,
	en_Weapon2
};

//////////////////////////////////////////////////////////////////////////

enum GameStateType
{
	en_Prepare_State = 0,
	en_Count_Down_State,
	en_Playing_State
};

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||