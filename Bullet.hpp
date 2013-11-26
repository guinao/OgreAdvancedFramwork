#ifndef BULLET_HPP
#define BULLET_HPP

#include "Ogre.h"

class Bullet
{
public:
	Bullet();
	~Bullet();

private:
	// Controls for the rocket/bullet/missle
	bool m_bFire;							// Fire up
	Ogre::Entity *m_pRocket;
	Ogre::SceneNode *m_pNode;
	int mCount;                        // The number of robots on the screen
	Ogre::Vector3 mRocketDirection;		// Direction the rocket flys
	Ogre::Real mFlySpeed;                 // The speed at which the object is moving 

	void setupParticles(Ogre::SceneNode* node);	// setup particle system for the node
	EnumWeaponType mCurrentWeapon;

};

#endif