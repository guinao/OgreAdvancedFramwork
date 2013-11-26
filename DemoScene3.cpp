//|||||||||||||||||||||||||||||||||||||||||||||||

#include "DemoScene3.hpp"
#include <cmath>

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

Ogre::String ps_name[] = {
//	"Examples/GreenyNimbus",
//	"Examples/PurpleFountain",
//	"Examples/Rain",
	"Examples/JetEngine1",
	"Examples/JetEngine2",
//	"Examples/Aureola",
//	"Examples/Swarm",
};

DemoScene3::DemoScene3()
{
	mDirection = Ogre::Vector3::UNIT_X;
	mDistance = 0.0;
	mFlySpeed = 3.0;
	mSelecting = false;
	mRaySceneQuery = 0;
	mVolQuery = 0;

	m_bLMouseDown       = false;
	m_bRMouseDown       = false;
	m_bQuit             = false;
	m_bSettingsMode     = false;

	m_pDetailsPanel	= 0;

	m_bFire = false;
	m_pCameraMan = 0;
	mCount = 0;

	// First-Person-View Camera variants
	mCameraNode = 0;
	mCameraYawNode = 0;
	mCameraPitchNode = 0;
	mCameraRollNode = 0;
	mRotX = 0.0;
	mRotY = 0.0;
	mCameraTranslateVector = Ogre::Vector3::ZERO;
	m_CamMoveScale = 5.0;
	mCameraRotateScale = 0.05;

	m_pRocket = 0;

	m_pTextOverlay = 0;
	m_pTextContainer = 0;
	m_pCountDownText = 0;
	m_bCountDownFinished = true;
	m_dCountDownSecond = 0.0;	    // Length of count down
	m_iCountDown = 100;

	m_pHudOverlay = 0;
	m_pHudPanel = 0;

	mCurrentWeapon = en_Weapon1;

	m_CurrentState = en_Prepare_State;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DemoScene3::enter()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering DemoScene3...");

	// Choose scene manager
	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "DemoScene3Mgr");

	// Create camera
	m_pCamera = m_pSceneMgr->createCamera("PlayerCam");

	// Create the camera's top node (which will only handle position).
	mCameraNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("CamNode");
	mCameraNode->setPosition(0, 250, 0);

	// Create the camera's yaw node as a child of camera's top node.
	this->mCameraYawNode = this->mCameraNode->createChildSceneNode();

	// Create the camera's pitch node as a child of camera's yaw node.
	this->mCameraPitchNode = this->mCameraYawNode->createChildSceneNode();

	// Create the camera's roll node as a child of camera's pitch node
	// and attach the camera to it.
	this->mCameraRollNode = this->mCameraPitchNode->createChildSceneNode();
	this->mCameraRollNode->attachObject(this->m_pCamera);

	// Create viewport
	OgreFramework::getSingletonPtr()->m_pViewport->setBackgroundColour(Ogre::ColourValue(0,0,0));
	m_pCamera->setAspectRatio(Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualWidth()) /
		Real(OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight()));
	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);

	// Create Volume query object
	mVolQuery = m_pSceneMgr->createPlaneBoundedVolumeQuery(Ogre::PlaneBoundedVolumeList());

	// Create selection box
	mSelectionBox = new SelectionBox("SelectionBox");
	m_pSceneMgr->getRootSceneNode()->createChildSceneNode()->attachObject(mSelectionBox);

	// Load Font for display
	Ogre::FontManager::getSingletonPtr()->load("StarWars", "Popular");
	Ogre::FontManager::getSingletonPtr()->load("BlueHigh", "Popular");
	Ogre::FontManager::getSingletonPtr()->load("BlueCond", "Popular");
	Ogre::FontManager::getSingletonPtr()->load("BlueBold", "Popular");

	buildGUI();

	// Display count down number
	createCountDownOverlay();
	createHudOverlay();

	createScene();
}

bool DemoScene3::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing DemoScene3...");

	m_pTextOverlay->hide();
	m_pHudOverlay->hide();

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DemoScene3::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming DemoScene3...");

	buildGUI();
	
	m_pHudOverlay->show();
	if(m_CurrentState == en_Count_Down_State)
	{
		m_pTextOverlay->show();
	}

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
	m_bQuit = false;
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving DemoScene3...");

	OGRE_DELETE mTerrainGroup;
	OGRE_DELETE mTerrainGlobals;

	if(mVolQuery)
	{
		m_pSceneMgr->destroyQuery(mVolQuery);
	}
	if(mSelectionBox){
		delete mSelectionBox;
	}
	if(m_pCameraMan){
		delete m_pCameraMan;
	}
	if(m_pCamera){
		m_pSceneMgr->destroyCamera(m_pCamera);
	}
	if (mRaySceneQuery)
	{
		m_pSceneMgr->destroyQuery(mRaySceneQuery);
	}
	if(m_pSceneMgr){
		OgreFramework::getSingletonPtr()->m_pRoot->destroySceneManager(m_pSceneMgr);
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::createScene()
{
	//m_pCamera->setPosition(Ogre::Vector3(1683, 50, 2116));
	//m_pCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
	m_pCamera->setPosition(Ogre::Vector3(0, 500, 0));
	m_pCamera->lookAt(Ogre::Vector3(-20, 50, -500));
	m_pCamera->setNearClipDistance(0.1);
	m_pCamera->setFarClipDistance(5000);

	if (OgreFramework::getSingletonPtr()->m_pRoot->getRenderSystem()->getCapabilities()->hasCapability(Ogre::RSC_INFINITE_FAR_PLANE))
	{
		m_pCamera->setFarClipDistance(0);   // enable infinite far clip distance if we can
	}


	// set the default lighting
	Ogre::Vector3 lightdir(0.55, -0.3, 0.75);
	lightdir.normalise();

	Ogre::Light* light = m_pSceneMgr->createLight("tstLight");
	light->setType(Ogre::Light::LT_POINT);
	light->setPosition(Ogre::Vector3(0, 150, 250));
	//light->setDirection(lightdir);
	light->setDiffuseColour(Ogre::ColourValue::White);
	light->setSpecularColour(Ogre::ColourValue(0.4, 0.4, 0.4));


	mTerrainGlobals = OGRE_NEW Ogre::TerrainGlobalOptions();
	mTerrainGroup = OGRE_NEW Ogre::TerrainGroup(m_pSceneMgr, Ogre::Terrain::ALIGN_X_Z, 513, 12000.0f);

	mTerrainGroup->setFilenameConvention(Ogre::String("BasicTutorial3Terrain"), Ogre::String("dat"));
	mTerrainGroup->setOrigin(Ogre::Vector3::ZERO);

	configureTerrainDefaults(light);

	for (long x = 0; x <= 0; ++x)
		for (long y = 0; y <= 0; ++y)
			defineTerrain(x, y);

	// sync load since we want everything in place when we start
	mTerrainGroup->loadAllTerrains(true);

	if (mTerrainsImported)
	{
		Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
		while(ti.hasMoreElements())
		{
			Ogre::Terrain* t = ti.getNext()->instance;
			initBlendMaps(t);
		}
	}

	mTerrainGroup->freeTemporaryResources();

	// Set up sky domes
	m_pSceneMgr->setSkyDome(true, "Examples/CloudySky", 5, 8);
}

//////////////////////////////////////////////////////////////////////////

// Display count down numbers
void DemoScene3::createCountDownOverlay()
{
	Ogre::OverlayManager *overlayMgr = Ogre::OverlayManager::getSingletonPtr();

	m_pTextOverlay = overlayMgr->create("TextOverlay");
	m_pTextContainer = static_cast<Ogre::OverlayContainer*>(overlayMgr->createOverlayElement("Panel", "TextContainer"));
	m_pTextContainer->setMetricsMode(Ogre::GMM_RELATIVE);
	m_pTextContainer->setDimensions( 1 , 1 );
	m_pTextContainer->setPosition( 0 , 0 );

	m_pCountDownText = static_cast<Ogre::TextAreaOverlayElement*>(overlayMgr->createOverlayElement("TextArea", "CountDownText"));
	m_pCountDownText->setMetricsMode(Ogre::GMM_RELATIVE);
	m_pCountDownText->setDimensions( 0.5 , 0.5 );
	m_pCountDownText->setPosition(0.45, 0.3);
	m_pCountDownText->setCaption("");
	m_pCountDownText->setCharHeight(0.5);
	m_pCountDownText->setAlignment(Ogre::TextAreaOverlayElement::Alignment::Center);
	m_pCountDownText->setFontName("StarWars");

	m_pTextContainer->addChild(m_pCountDownText);

	m_pTextOverlay->add2D(m_pTextContainer);
	m_pTextOverlay->setZOrder(300);

//	m_pTextOverlay->show();
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::createHudOverlay()
{
	Ogre::OverlayManager *overlayMgr = Ogre::OverlayManager::getSingletonPtr();
	
	m_pHudPanel = static_cast<Ogre::PanelOverlayElement*>(overlayMgr->createOverlayElement("Panel", "HudPanel"));
	m_pHudPanel->setMetricsMode(Ogre::GMM_PIXELS);
	m_pHudPanel->setHorizontalAlignment(Ogre::GuiHorizontalAlignment::GHA_CENTER);
	m_pHudPanel->setVerticalAlignment(Ogre::GuiVerticalAlignment::GVA_CENTER);
	
	int height = OgreFramework::getSingletonPtr()->m_pViewport->getActualHeight();
	m_pHudPanel->setPosition(-127, height/2-168);
	m_pHudPanel->setDimensions(254, 168);
	m_pHudPanel->setMaterialName("MyMaterials/APanelMaterial");

	m_pHudOverlay = overlayMgr->create("HudOverlay");
	m_pHudOverlay->add2D(m_pHudPanel);

	m_pHudOverlay->show();
}

//////////////////////////////////////////////////////////////////////////

bool DemoScene3::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	//m_pCameraMan->injectKeyDown(keyEventRef);

	if(m_bSettingsMode == true)
	{
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
		{
			OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("ChatModeSelMenu");
			if(pMenu->getSelectionIndex() + 1 < (int)pMenu->getNumItems())
				pMenu->selectItem(pMenu->getSelectionIndex() + 1);
		}

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
		{
			OgreBites::SelectMenu* pMenu = (OgreBites::SelectMenu*)OgreFramework::getSingletonPtr()->m_pTrayMgr->getWidget("ChatModeSelMenu");
			if(pMenu->getSelectionIndex() - 1 >= 0)
				pMenu->selectItem(pMenu->getSelectionIndex() - 1);
		}
	}

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_ESCAPE))
	{
		pushAppState(findByName("PauseState"));
		return true;
	}

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_I))
	{
		if(m_pDetailsPanel->getTrayLocation() == OgreBites::TL_NONE)
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->moveWidgetToTray(m_pDetailsPanel, OgreBites::TL_TOPLEFT, 0);
			m_pDetailsPanel->show();
		}
		else
		{
			OgreFramework::getSingletonPtr()->m_pTrayMgr->removeWidgetFromTray(m_pDetailsPanel);
			m_pDetailsPanel->hide();
		}
	}

	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_TAB))
	{
		m_bSettingsMode = !m_bSettingsMode;
		return true;
	}

	if(m_bSettingsMode && OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RETURN) ||
		OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_NUMPADENTER))
	{
	}

	if(!m_bSettingsMode || (m_bSettingsMode && !OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O)))
		OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoScene3::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	//m_pCameraMan->injectKeyUp(keyEventRef);

	return true;
}

//////////////////////////////////////////////////////////////////////////

bool DemoScene3::mouseMoved(const OIS::MouseEvent &evt)
{
	if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;

	this->mRotX = -evt.state.X.rel * 0.01f;
	this->mRotY = -evt.state.Y.rel * 0.01f;

	Ogre::Real pitchAngle;
	Ogre::Real pitchAngleSign;

	// Yaws the camera according to the mouse relative movement.
	this->mCameraYawNode->yaw(this->mRotX);

	// Pitches the camera according to the mouse relative movement.
	this->mCameraPitchNode->pitch(this->mRotY);

	// Angle of rotation around the X-axis.
	pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(this->mCameraPitchNode->getOrientation().w)).valueDegrees());

	// Just to determine the sign of the angle we pick up above, the
	// value itself does not interest us.
	pitchAngleSign = this->mCameraPitchNode->getOrientation().x;

	// Limit the pitch between -90 degress and +90 degrees, Quake3-style.
	if (pitchAngle > 90.0f)
	{
		if (pitchAngleSign > 0)
			// Set orientation to 90 degrees on X-axis.
			this->mCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
			Ogre::Math::Sqrt(0.5f), 0, 0));
		else if (pitchAngleSign < 0)
			// Sets orientation to -90 degrees on X-axis.
			this->mCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
			-Ogre::Math::Sqrt(0.5f), 0, 0));
	}
	//m_pCameraMan->injectMouseMove(evt);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoScene3::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;

	//m_pCameraMan->injectMouseDown(evt, id);

	if(id == OIS::MB_Left)
	{
		onLeftPressed(evt);
		m_bLMouseDown = true;
	}
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = true;
	}

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoScene3::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseUp(evt, id)) return true;

	if(id == OIS::MB_Left)
	{
		m_bLMouseDown = false;

	}
	else if(id == OIS::MB_Right)
	{
		m_bRMouseDown = false;
	}

	//m_pCameraMan->injectMouseUp(evt, id);

	return true;
}

void DemoScene3::onLeftPressed(const OIS::MouseEvent &evt)
{	
	char name[16];
	sprintf(name, "Item%d", mCount++);
	m_pRocket = m_pSceneMgr->createEntity(name, "cube.mesh");
	m_pNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode(std::string(name)+"Node");
	m_pNode->attachObject(m_pRocket);
	m_pNode->scale(0.3, 0.3, 0.3);
	m_pNode->setPosition(m_pCamera->getRealPosition());
	//adjustHeight(m_pRocket);
	m_bFire = true;

	Ogre::Real clickX = static_cast<Ogre::Real>(evt.state.X.abs) / evt.state.width;
	Ogre::Real clickY = static_cast<Ogre::Real>(evt.state.Y.abs) / evt.state.height;
	Ogre::Ray mouseRay = m_pCamera->getCameraToViewportRay(clickX, clickY);

	Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	while(ti.hasMoreElements())
	{
		Ogre::Terrain* t = ti.getNext()->instance;
		std::pair<bool, Ogre::Vector3> result =  t->rayIntersects(mouseRay);
		if(result.first){
			mDestination = result.second;
			mRocketDirection = mDestination - m_pNode->getPosition();
			mDistance = mRocketDirection.normalise();
			break;
		}
	}

	setupParticles(m_pNode);		// Add particle effect
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::moveCamera()
{
	//Ogre::Real pitchAngle;
	//Ogre::Real pitchAngleSign;

	//// Yaws the camera according to the mouse relative movement.
	//this->mCameraYawNode->yaw(this->mRotX);

	//// Pitches the camera according to the mouse relative movement.
	//this->mCameraPitchNode->pitch(this->mRotY);

	// Translates the camera according to the translate vector which is
	// controlled by the keyboard arrows.
	//
	// NOTE: We multiply the mmCameraTranslateVector by the cameraPitchNode's
	// orientation quaternion and the cameraYawNode's orientation
	// quaternion to translate the camera accoding to the camera's
	// orientation around the Y-axis and the X-axis.
	this->mCameraNode->translate(this->mCameraYawNode->getOrientation() *
		this->mCameraPitchNode->getOrientation() *
		this->mCameraTranslateVector,
		Ogre::SceneNode::TS_LOCAL);

	mCameraTranslateVector = Ogre::Vector3::ZERO;

	adjustHeight(mCameraNode, 10.0);

	//// Angle of rotation around the X-axis.
	//pitchAngle = (2 * Ogre::Degree(Ogre::Math::ACos(this->mCameraPitchNode->getOrientation().w)).valueDegrees());

	//// Just to determine the sign of the angle we pick up above, the
	//// value itself does not interest us.
	//pitchAngleSign = this->mCameraPitchNode->getOrientation().x;

	//// Limit the pitch between -90 degress and +90 degrees, Quake3-style.
	//if (pitchAngle > 90.0f)
	//{
	//	if (pitchAngleSign > 0)
	//		// Set orientation to 90 degrees on X-axis.
	//		this->mCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
	//		Ogre::Math::Sqrt(0.5f), 0, 0));
	//	else if (pitchAngleSign < 0)
	//		// Sets orientation to -90 degrees on X-axis.
	//		this->mCameraPitchNode->setOrientation(Ogre::Quaternion(Ogre::Math::Sqrt(0.5f),
	//		-Ogre::Math::Sqrt(0.5f), 0, 0));
	//}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::getInput()
{
	if(m_bSettingsMode == false)
	{
		processUnbufferedKeyInputForCamera();
	}
}

//////////////////////////////////////////////////////////////////////////
void DemoScene3::processUnbufferedKeyInputForCamera()
{
	Ogre::Real moveFactor = 1.0;
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT)
		|| OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RSHIFT))
		moveFactor = 10.0;

	// Move camera upwards along to world's Y-axis.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_PGUP))
		//this->mCameraTranslateVector.y = this->moveScale;
		this->mCameraNode->setPosition(this->mCameraNode->getPosition() + Ogre::Vector3(0, 5, 0));

	// Move camera downwards along to world's Y-axis.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_PGDOWN))
		//this->mCameraTranslateVector.y = -(this->moveScale);
		this->mCameraNode->setPosition(this->mCameraNode->getPosition() - Ogre::Vector3(0, 5, 0));

	// Move camera forward.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_UP))
		this->mCameraTranslateVector.z = -(this->m_CamMoveScale * moveFactor);

	// Move camera backward.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_DOWN))
		this->mCameraTranslateVector.z = this->m_CamMoveScale * moveFactor;

	// Move camera left.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LEFT))
		this->mCameraTranslateVector.x = -(this->m_CamMoveScale * moveFactor);

	// Move camera right.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_RIGHT))
		this->mCameraTranslateVector.x = this->m_CamMoveScale * moveFactor;

	// Rotate camera left.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Q))
		this->mCameraYawNode->yaw(this->mCameraRotateScale);

	// Rotate camera right.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
		this->mCameraYawNode->yaw(-(this->mCameraRotateScale));

	// Strip all yaw rotation on the camera.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
		this->mCameraYawNode->setOrientation(Ogre::Quaternion::IDENTITY);

	// Rotate camera upwards.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Z))
		this->mCameraPitchNode->pitch(this->mCameraRotateScale);

	// Rotate camera downwards.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
		this->mCameraPitchNode->pitch(-(this->mCameraRotateScale));

	// Strip all pitch rotation on the camera.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_E))
		this->mCameraPitchNode->setOrientation(Ogre::Quaternion::IDENTITY);

	// Tilt camera on the left.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_L))
		this->mCameraRollNode->roll(-(this->mCameraRotateScale));

	// Tilt camera on the right.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_M))
		this->mCameraRollNode->roll(this->mCameraRotateScale);

	// Strip all tilt applied to the camera.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_P))
		this->mCameraRollNode->setOrientation(Ogre::Quaternion::IDENTITY);

	// Strip all rotation to the camera.
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_O))
	{
		this->mCameraYawNode->setOrientation(Ogre::Quaternion::IDENTITY);
		this->mCameraPitchNode->setOrientation(Ogre::Quaternion::IDENTITY);
		this->mCameraRollNode->setOrientation(Ogre::Quaternion::IDENTITY);
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::update(double timeSinceLastFrame)
{
	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

	// exit scene
	if(m_bQuit == true)
	{
		popAppState();
		return;
	}

	// update display messages
	if(!OgreFramework::getSingletonPtr()->m_pTrayMgr->isDialogVisible())
	{
		if(m_pDetailsPanel->isVisible())
		{
			m_pDetailsPanel->setParamValue(0, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().x));
			m_pDetailsPanel->setParamValue(1, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().y));
			m_pDetailsPanel->setParamValue(2, Ogre::StringConverter::toString(m_pCamera->getDerivedPosition().z));
			m_pDetailsPanel->setParamValue(3, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().w));
			m_pDetailsPanel->setParamValue(4, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().x));
			m_pDetailsPanel->setParamValue(5, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().y));
			m_pDetailsPanel->setParamValue(6, Ogre::StringConverter::toString(m_pCamera->getDerivedOrientation().z));
			if(m_bSettingsMode)
				m_pDetailsPanel->setParamValue(7, "Buffered Input");
			else
				m_pDetailsPanel->setParamValue(7, "Un-Buffered Input");
		}
	}

	switch(m_CurrentState)
	{
	case en_Prepare_State:
		// No Operation
		break;
	case en_Count_Down_State:
		updateInCountDownState(timeSinceLastFrame);
		break;
	case en_Playing_State:
		updateInPlayingState(timeSinceLastFrame);
		break;
	default:
		break;
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::updateInPlayingState(double timeSinceLastFrame)
{
	getInput();

	moveCamera();

	//////////////////////////////////////////////////////////////////////////
	// move rocket
	if(m_bFire)
	{
		//////////////////////////////////////////////////////////////////////////
		// update the rocket position
		Ogre::Real move = mFlySpeed * timeSinceLastFrame;
		mDistance -= move;
		if(mDistance <= 0.0)
		{
			m_pNode->setPosition(mDestination);
			mRocketDirection = Ogre::Vector3::ZERO;
			m_pSceneMgr->getParticleSystem(m_pNode->getName()+"PS")->setVisible(false);
			m_bFire = false;
		}
		else
		{
			m_pNode->translate(mRocketDirection * move);
			//adjustHeight(m_pRocket);
		}
		//////////////////////////////////////////////////////////////////////////

		//////////////////////////////////////////////////////////////////////////
		// Collision test

		// Setup AABB scene query for the object
		Ogre::AxisAlignedBoxSceneQuery* pQuery = m_pSceneMgr->createAABBQuery(m_pRocket->getWorldBoundingBox());

		//Execute the query
		Ogre::SceneQueryResult result = pQuery->execute();

		// Iterate objects in the query results
		Ogre::SceneQueryResultMovableList::iterator iter;
		bool collision = false;
		for(iter = result.movables.begin(); iter!=result.movables.end(); ++iter)
		{
			Ogre::MovableObject* pObject = static_cast<Ogre::MovableObject*>(*iter);
			if(pObject)
			{
				if(pObject->getMovableType() == "Entity")
				{
					Ogre::Entity* ent = static_cast<Ogre::Entity*>(pObject);

					if(ent->getName() != m_pRocket->getName())
					{
						ent->setVisible(false);
						m_pSceneMgr->destroyEntity(ent);
						collision = true;
					}
				}
			}
		}

		if(collision)
		{
			m_pRocket->setVisible(false);
			m_pSceneMgr->destroyEntity(m_pRocket);
			m_bFire = false;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::updateInCountDownState(double timeSinceLastFrame)
{
	char str[10];

	m_dCountDownSecond += timeSinceLastFrame;
	if(m_iCountDown > 0)
	{
		if(m_dCountDownSecond > 0.1)
		{
			--m_iCountDown;
			m_dCountDownSecond = 0.0;
		}
		sprintf(str, "%d", (m_iCountDown+9)/10);
		if( strcmp(str, "0") == 0)
		{
			strcpy(str, "GO!");
		}

		m_pCountDownText->setCaption(str);
	}
	else
	{
		m_bCountDownFinished = true;

		m_pTextOverlay->hide();

		m_CurrentState = en_Playing_State;
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::buildGUI()
{
	OgreFramework::getSingletonPtr()->m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
//	OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "GameLbl", "DemoScene3", 250);
//	OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

	Ogre::StringVector items;
	items.push_back("cam.pX");
	items.push_back("cam.pY");
	items.push_back("cam.pZ");
	items.push_back("cam.oW");
	items.push_back("cam.oX");
	items.push_back("cam.oY");
	items.push_back("cam.oZ");
	items.push_back("Mode");

	m_pDetailsPanel = OgreFramework::getSingletonPtr()->m_pTrayMgr->createParamsPanel(OgreBites::TL_TOPLEFT, "DetailsPanel", 200, items);
	m_pDetailsPanel->show();

	Ogre::String infoText = "[TAB] - Switch input mode\n\n[W] - Forward / Mode up\n[S] - Backwards/ Mode down\n[A] - Left\n";
	infoText.append("[D] - Right\n\nPress [SHIFT] to move faster\n\n[O] - Toggle FPS / logo\n");
	infoText.append("[Print] - Take screenshot\n\n[ESC] - Exit");
	//OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_RIGHT, "InfoPanel", infoText, 300, 220);

	Ogre::StringVector chatModes;
	chatModes.push_back("Solid mode");
	chatModes.push_back("Wireframe mode");
	chatModes.push_back("Point mode");
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "ChatModeSelMenu", "ChatMode", 200, 3, chatModes);

	// Create Start Button
	if(m_CurrentState == en_Prepare_State)
	{
		OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_CENTER, "EnterBtn", "Start", 250);
	}
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "Weapon1Btn", "Weapon1", 250);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createButton(OgreBites::TL_BOTTOMRIGHT, "Weapon2Btn", "Weapon2", 250);

}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::itemSelected(OgreBites::SelectMenu* menu)
{
	switch(menu->getSelectionIndex())
	{
	case 0:
		m_pCamera->setPolygonMode(Ogre::PM_SOLID);break;
	case 1:
		m_pCamera->setPolygonMode(Ogre::PM_WIREFRAME);break;
	case 2:
		m_pCamera->setPolygonMode(Ogre::PM_POINTS);break;
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::buttonHit(OgreBites::Button *button)
{
	if(button->getName() == "EnterBtn"){
		m_bCountDownFinished = false;
		button->hide();
		OgreFramework::getSingletonPtr()->m_pTrayMgr->destroyWidget(button);
		m_pTextOverlay->show();
		m_CurrentState = en_Count_Down_State;
	}
	else if(button->getName() == "Weapon1Btn"){
		mCurrentWeapon = en_Weapon1;
	}
	else if(button->getName() == "Weapon2Btn"){
		mCurrentWeapon = en_Weapon2;
	}
}

//////////////////////////////////////////////////////////////////////////

//-------------------------------------------------------------------------------------
static void getTerrainImage(bool flipX, bool flipY, Ogre::Image& img)
{
	img.load("terrain.png", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME);
	if (flipX)
		img.flipAroundY();
	if (flipY)
		img.flipAroundX();
}
//-------------------------------------------------------------------------------------
void DemoScene3::defineTerrain(long x, long y)
{
	Ogre::String filename = mTerrainGroup->generateFilename(x, y);
	if (Ogre::ResourceGroupManager::getSingleton().resourceExists(mTerrainGroup->getResourceGroup(), filename))
	{
		mTerrainGroup->defineTerrain(x, y);
	}
	else
	{
		Ogre::Image img;
		getTerrainImage(x % 2 != 0, y % 2 != 0, img);
		mTerrainGroup->defineTerrain(x, y, &img);
		mTerrainsImported = true;
	}
}
//-------------------------------------------------------------------------------------
void DemoScene3::initBlendMaps(Ogre::Terrain* terrain)
{
	Ogre::TerrainLayerBlendMap* blendMap0 = terrain->getLayerBlendMap(1);
	Ogre::TerrainLayerBlendMap* blendMap1 = terrain->getLayerBlendMap(2);
	Ogre::Real minHeight0 = 70;
	Ogre::Real fadeDist0 = 40;
	Ogre::Real minHeight1 = 70;
	Ogre::Real fadeDist1 = 15;
	float* pBlend0 = blendMap0->getBlendPointer();
	float* pBlend1 = blendMap1->getBlendPointer();
	for (Ogre::uint16 y = 0; y < terrain->getLayerBlendMapSize(); ++y)
	{
		for (Ogre::uint16 x = 0; x < terrain->getLayerBlendMapSize(); ++x)
		{
			Ogre::Real tx, ty;

			blendMap0->convertImageToTerrainSpace(x, y, &tx, &ty);
			Ogre::Real height = terrain->getHeightAtTerrainPosition(tx, ty);
			Ogre::Real val = (height - minHeight0) / fadeDist0;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend0++ = val;

			val = (height - minHeight1) / fadeDist1;
			val = Ogre::Math::Clamp(val, (Ogre::Real)0, (Ogre::Real)1);
			*pBlend1++ = val;
		}
	}
	blendMap0->dirty();
	blendMap1->dirty();
	blendMap0->update();
	blendMap1->update();
}
//-------------------------------------------------------------------------------------
void DemoScene3::configureTerrainDefaults(Ogre::Light* light)
{
	// Configure global
	mTerrainGlobals->setMaxPixelError(8);
	// testing composite map
	mTerrainGlobals->setCompositeMapDistance(3000);

	// Important to set these so that the terrain knows what to use for derived (non-realtime) data
	mTerrainGlobals->setLightMapDirection(light->getDerivedDirection());
	mTerrainGlobals->setCompositeMapAmbient(m_pSceneMgr->getAmbientLight());
	mTerrainGlobals->setCompositeMapDiffuse(light->getDiffuseColour());

	// Configure default import settings for if we use imported image
	Ogre::Terrain::ImportData& defaultimp = mTerrainGroup->getDefaultImportSettings();
	defaultimp.terrainSize = 513;
	defaultimp.worldSize = 12000.0f;
	defaultimp.inputScale = 600; // due terrain.png is 8 bpp
	defaultimp.minBatchSize = 33;
	defaultimp.maxBatchSize = 65;

	// textures
	defaultimp.layerList.resize(3);
	defaultimp.layerList[0].worldSize = 100;
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_diffusespecular.dds");
	defaultimp.layerList[0].textureNames.push_back("dirt_grayrocky_normalheight.dds");
	defaultimp.layerList[1].worldSize = 30;
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_diffusespecular.dds");
	defaultimp.layerList[1].textureNames.push_back("grass_green-01_normalheight.dds");
	defaultimp.layerList[2].worldSize = 200;
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_diffusespecular.dds");
	defaultimp.layerList[2].textureNames.push_back("growth_weirdfungus-03_normalheight.dds");
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::adjustHeight(Ogre::Entity* ent)
{
	Ogre::Vector3 pos = ent->getParentNode()->getPosition();
	Ogre::Ray ray(Vector3(pos.x, 5000.0f, pos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);

	Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	while(ti.hasMoreElements())
	{
		Ogre::Terrain* t = ti.getNext()->instance;
		std::pair<bool, Ogre::Vector3> result =  t->rayIntersects(ray);
		if(result.first){
			ent->getParentNode()->setPosition(result.second);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::adjustHeight(Ogre::SceneNode* node, Ogre::Real height)
{
	Ogre::Vector3 pos = node->getPosition();
	Ogre::Ray ray(Vector3(pos.x, 5000.0f, pos.z), Ogre::Vector3::NEGATIVE_UNIT_Y);

	Ogre::TerrainGroup::TerrainIterator ti = mTerrainGroup->getTerrainIterator();
	while(ti.hasMoreElements())
	{
		Ogre::Terrain* t = ti.getNext()->instance;
		std::pair<bool, Ogre::Vector3> result =  t->rayIntersects(ray);
		if(result.first){
			node->setPosition(result.second.x, result.second.y+height, result.second.z);
			break;
		}
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene3::setupParticles(Ogre::SceneNode* node)
{
	Ogre::ParticleSystem* ps;

	// create aureola around ogre head perpendicular to the ground
	switch(mCurrentWeapon)
	{
	case en_Weapon1:
		ps = m_pSceneMgr->createParticleSystem(node->getName()+"PS", ps_name[0]);
		break;
	case en_Weapon2:
		ps = m_pSceneMgr->createParticleSystem(node->getName()+"PS", ps_name[1]);
		break;
	default:
		ps = m_pSceneMgr->createParticleSystem(node->getName()+"PS", ps_name[0]);
		break;
	}
	node->attachObject(ps);

	// rotate the node so the particle is at tail
	Ogre::Quaternion quat = Vector3(0, -1, 0).getRotationTo(-mRocketDirection);
	node->rotate(quat);
}
