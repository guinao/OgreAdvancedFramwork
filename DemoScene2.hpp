//|||||||||||||||||||||||||||||||||||||||||||||||

#ifndef DEMO_SCENE_2_HPP
#define DEMO_SCENE_2_HPP

//|||||||||||||||||||||||||||||||||||||||||||||||

#include "AppState.hpp"

#include "DotSceneLoader.hpp"
#include "Constants.h"
#include "SelectionBox.hpp"
#include "SinbadCharacterController.hpp"

#include <deque>

#include <OgreSubEntity.h>
#include <OgreMaterialManager.h>
#include <Terrain/OgreTerrain.h>
#include <Terrain/OgreTerrainGroup.h>

#include <SdkCameraMan.h>

//|||||||||||||||||||||||||||||||||||||||||||||||

//enum QueryFlags
//{
//	OGRE_HEAD_MASK	= 1<<0,
//	CUBE_MASK		= 1<<1
//};

//|||||||||||||||||||||||||||||||||||||||||||||||

class DemoScene2 : public AppState
{
public:
	DemoScene2();

	DECLARE_APPSTATE_CLASS(DemoScene2)

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
	void itemSelected(OgreBites::SelectMenu* menu);

	void update(double timeSinceLastFrame);

protected:

private:
	void defineTerrain(long x, long y);
	void initBlendMaps(Ogre::Terrain* terrain);
	void configureTerrainDefaults(Ogre::Light* light);

	void adjustHeight(Ogre::Entity* ent);				// Adjust the height of the entity to be on the terrian

	OgreBites::ParamsPanel*		m_pDetailsPanel;
	bool				m_bQuit;

	bool				m_bLMouseDown, m_bRMouseDown;
	bool				m_bSettingsMode;

	Ogre::Real mDistance;                  // The distance the object has left to travel
	Ogre::Vector3 mDirection;              // The direction the object is moving
	Ogre::Vector3 mDestination;            // The destination the object is moving towards

	Ogre::AnimationState *mAnimationState; // The current animation state of the object

	Ogre::Entity *mEntity;                 // The Entity we are animating
	Ogre::SceneNode *mNode;                // The SceneNode that the Entity is attached to
	std::deque<Ogre::Vector3> mWalkList;   // The list of points we are walking to

	Ogre::Real mWalkSpeed;                 // The speed at which the object is moving 

	Ogre::SceneNode *cameraNode;
	Ogre::SceneNode *cameraYawNode;
	Ogre::SceneNode *cameraPitchNode;
	Ogre::SceneNode *cameraRollNode;

	Ogre::Real  mRotate;
	Ogre::Real mTimeStep;

	Ogre::TerrainGlobalOptions* mTerrainGlobals;
	Ogre::TerrainGroup* mTerrainGroup;
	bool mTerrainsImported;
	bool mIsWalking;
	bool mIsDead;

	Ogre::Real m_MoveScale;
	Ogre::RaySceneQuery *mRaySceneQuery;     // The ray scene query pointer
	bool mLMouseDown, mRMouseDown;     // True if the mouse buttons are down
	int mCount;                        // The number of robots on the screen
	Ogre::SceneNode *mCurrentObject;         // The newly created object
	float mRotateSpeed;

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

	Ogre::SceneNode* mCamNode3;

	OgreBites::SdkCameraMan* m_pCameraMan;

	SinbadCharacterController* mChara;

	void createSinbad();

	// variables for sinbad
	enum AnimID
	{
		ANIM_IDLE_BASE,
		ANIM_IDLE_TOP,
		ANIM_RUN_BASE,
		ANIM_RUN_TOP,
		ANIM_HANDS_CLOSED,
		ANIM_HANDS_RELAXED,
		ANIM_DRAW_SWORDS,
		ANIM_SLICE_VERTICAL,
		ANIM_SLICE_HORIZONTAL,
		ANIM_DANCE,
		ANIM_JUMP_START,
		ANIM_JUMP_LOOP,
		ANIM_JUMP_END,
		ANIM_NONE
	};


	Ogre::SceneNode* mBodyNode;
	Ogre::SceneNode* mCameraPivot;
	Ogre::SceneNode* mCameraGoal;
	Ogre::SceneNode* mCameraNode;
	Ogre::Real mPivotPitch;
	Ogre::Entity* mBodyEnt;
	Ogre::Entity* mSword1;
	Ogre::Entity* mSword2;
	Ogre::RibbonTrail* mSwordTrail;
	Ogre::AnimationState* mAnims[NUM_ANIMS];    // master animation list
	AnimID mBaseAnimID;                   // current base (full- or lower-body) animation
	AnimID mTopAnimID;                    // current top (upper-body) animation
	bool mFadingIn[NUM_ANIMS];            // which animations are fading in
	bool mFadingOut[NUM_ANIMS];           // which animations are fading out
	bool mSwordsDrawn;
	Ogre::Vector3 mKeyDirection;      // player's local intended direction based on WASD keys
	Ogre::Vector3 mGoalDirection;     // actual intended direction in world-space
	Ogre::Real mVerticalVelocity;     // for jumping
	Ogre::Real mTimer;                // general timer to see how long animations have been playing

};

//|||||||||||||||||||||||||||||||||||||||||||||||

#endif

//|||||||||||||||||||||||||||||||||||||||||||||||