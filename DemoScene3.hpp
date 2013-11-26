//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef DEMO_SCENE_3_HPP
#define DEMO_SCENE_3_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

#include "DotSceneLoader.hpp"
#include "Constants.h"
#include "SelectionBox.hpp"
#include "SinbadCharacterController.hpp"
#include "OgreTextAreaOverlayElement.h"

#include <deque>

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <OgreTextAreaOverlayElement.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include <SdkCameraMan.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

//enum QueryFlags
//{
//	OGRE_HEAD_MASK	= 1<<0,
//	CUBE_MASK		= 1<<1
//};

//////////////////////////////////////////////////////////////////////////

class DemoScene3 : public AppState
{
public:
	DemoScene3();

	DECLARE_APPSTATE_CLASS(DemoScene3)

	void enter();
	void createScene();
	void exit();
	bool pause();
	void resume();

	void moveCamera();
	void getInput();
	void buildGUI();

	bool keyPressed(const OIS::KeyEvent &keyEventRef);
	bool keyReleased(const OIS::KeyEvent &keyEventRef);

	bool mouseMoved(const OIS::MouseEvent &evt);
	bool mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id);
	bool mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id);

	void onLeftPressed(const OIS::MouseEvent &evt);

	// GUI events
	void itemSelected(OgreBites::SelectMenu* menu);
	void buttonHit(OgreBites::Button* button);

	void update(double timeSinceLastFrame);

protected:

private:
	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain* terrain);
	void configureTerrainDefaults(Ogre::Light* light);

	void adjustHeight(Ogre::Entity* ent);				// Adjust the height of the entity to be on the terrian

	// Adjust the height of the SceneNode to be height units above the terrian
	void adjustHeight(Ogre::SceneNode* node, Ogre::Real height);

	OgreBites::ParamsPanel*		m_pDetailsPanel;
	bool				m_bQuit;

	bool				m_bLMouseDown, m_bRMouseDown;
	bool				m_bSettingsMode;

	Ogre::Real mDistance;                  // The distance the object has left to travel
	Ogre::Vector3 mDirection;              // The direction the object is moving
	Ogre::Vector3 mDestination;            // The destination the object is moving towards

	Ogre::TerrainGlobalOptions* mTerrainGlobals;
	Ogre::TerrainGroup* mTerrainGroup;
	bool mTerrainsImported;
	bool mIsWalking;
	bool mIsDead;

	Ogre::RaySceneQuery *mRaySceneQuery;     // The ray scene query pointer
	bool mLMouseDown, mRMouseDown;     // True if the mouse buttons are down

	Ogre::SceneNode *mCurrentObject;         // The newly created object

	// Cursor
	Ogre::Overlay* OverlayCursor;
	Ogre::PanelOverlayElement* mCursor;

	bool bRobotMode;        // The current state

	// Intermediate Tutorial 4
	Ogre::Vector2 mStart, mStop;
	Ogre::PlaneBoundedVolumeListSceneQuery* mVolQuery;
	std::list<Ogre::MovableObject*> mSelected;
	bool mSelecting;
	SelectionBox* mSelectionBox;
	//////////////////////////////////////////////////////////////////////////

	// Creating a simple first-person camera system
	Ogre::SceneNode *mCameraNode;
	Ogre::SceneNode *mCameraYawNode;
	Ogre::SceneNode *mCameraPitchNode;
	Ogre::SceneNode *mCameraRollNode;
	
	Ogre::Radian mRotY;
	Ogre::Radian mRotX;
	Ogre::Radian mCameraRotateScale;

	Ogre::Vector3 mCameraTranslateVector;

	Ogre::Real m_CamMoveScale;

	void processUnbufferedKeyInputForCamera();

	//////////////////////////////////////////////////////////////////////////

	OgreBites::SdkCameraMan* m_pCameraMan;

	//////////////////////////////////////////////////////////////////////////

	// Controls for the rocket/bullet/missle
	bool m_bFire;							// Fire up
	Ogre::Entity *m_pRocket;
	Ogre::SceneNode *m_pNode;
	int mCount;                        // The number of robots on the screen
	Ogre::Vector3 mRocketDirection;		// Direction the rocket flys
	Ogre::Real mFlySpeed;                 // The speed at which the object is moving 

	void setupParticles(Ogre::SceneNode* node);	// setup particle system for the node
	EnumWeaponType mCurrentWeapon;

	//////////////////////////////////////////////////////////////////////////

	// Text Overlay for count down
	Ogre::Overlay *m_pTextOverlay;
	Ogre::OverlayContainer *m_pTextContainer;
	Ogre::TextAreaOverlayElement *m_pCountDownText;
	double  m_dCountDownSecond;
	int m_iCountDown;
	bool m_bCountDownFinished;

	void createCountDownOverlay();

	// Overlay for HUD
	Ogre::Overlay *m_pHudOverlay;
//	Ogre::OverlayContainer *m_pHudContainter;
	Ogre::PanelOverlayElement *m_pHudPanel;

	void createHudOverlay();
	
	//////////////////////////////////////////////////////////////////////////

	// Game logic control
	GameStateType m_CurrentState;
	void updateInPlayingState(double timeSinceLastFrame);
	void updateInCountDownState(double timeSinceLastFrame);
};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||