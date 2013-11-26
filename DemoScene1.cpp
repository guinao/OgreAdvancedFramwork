//|||||||||||||||||||||||||||||||||||||||||||||||

#include "DemoScene1.hpp"

//|||||||||||||||||||||||||||||||||||||||||||||||

using namespace Ogre;

//|||||||||||||||||||||||||||||||||||||||||||||||

DemoScene1::DemoScene1()
{

	// Create the walking list
	mWalkList.push_back(Ogre::Vector3(550.0f,  0.0f,  50.0f ));
	mWalkList.push_back(Ogre::Vector3(-100.0f,  0.0f, -200.0f));

	mDirection = Ogre::Vector3::UNIT_X;
	mWalkSpeed = 10.0;
	mRotateSpeed = 0.3;
	mIsWalking = false;
	mIsDead = false;
	bRobotMode = true;
	mSelecting = false;
	mRaySceneQuery = 0;
	mVolQuery = 0;

	m_bLMouseDown       = false;
	m_bRMouseDown       = false;
	m_bQuit             = false;
	m_bSettingsMode     = false;

	m_pDetailsPanel	= 0;

	mChara = 0;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DemoScene1::enter()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Entering DemoScene1...");

	// Choose scene manager
	m_pSceneMgr = OgreFramework::getSingletonPtr()->m_pRoot->createSceneManager(ST_GENERIC, "DemoScene1Mgr");

	// Create camera
	m_pCamera = m_pSceneMgr->createCamera("PlayerCam");
	m_pCameraMan = new OgreBites::SdkCameraMan(m_pCamera);
	//m_pCameraMan->setStyle(OgreBites::CS_FREELOOK);

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

	buildGUI();

	createScene();
}

bool DemoScene1::pause()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Pausing DemoScene1...");

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

void DemoScene1::resume()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Resuming DemoScene1...");

	buildGUI();

	OgreFramework::getSingletonPtr()->m_pViewport->setCamera(m_pCamera);
	m_bQuit = false;
}

//////////////////////////////////////////////////////////////////////////

void DemoScene1::exit()
{
	OgreFramework::getSingletonPtr()->m_pLog->logMessage("Leaving DemoScene1...");

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

void DemoScene1::createScene()
{
	m_pCamera->setPosition(Ogre::Vector3(2000, 50, 2116));
	m_pCamera->lookAt(Ogre::Vector3(1963, 50, 1660));
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

	mChara = new SinbadCharacterController(m_pCamera);

	adjustHeight(mChara->getEntity());
	mChara->getEntity()->getParentNode()->translate(Vector3::UNIT_Y * CHAR_HEIGHT);

	m_pCameraMan->setTarget(mChara->getEntity()->getParentSceneNode());

	//createSinbad();
}

//////////////////////////////////////////////////////////////////////////

void DemoScene1::createSinbad()
{
// part 1 setup body
	mBodyNode = m_pSceneMgr->getRootSceneNode()->createChildSceneNode("SinbadNode", Vector3::UNIT_Y * CHAR_HEIGHT);
	mBodyNode->setPosition(Ogre::Vector3(1683, 50, 2116));

	mBodyEnt = m_pSceneMgr->createEntity("SinbadBody", "Sinbad.mesh");
	mBodyNode->attachObject(mBodyEnt);
	adjustHeight(mBodyEnt);
	mBodyNode->translate(Vector3::UNIT_Y * CHAR_HEIGHT);

	// create swords and attach to sheath
	mSword1 = m_pSceneMgr->createEntity("SinbadSword1", "Sword.mesh");
	mSword2 = m_pSceneMgr->createEntity("SinbadSword2", "Sword.mesh");
	mBodyEnt->attachObjectToBone("Sheath.L", mSword1);
	mBodyEnt->attachObjectToBone("Sheath.R", mSword2);

	LogManager::getSingleton().logMessage("Creating the chains");
	// create a couple of ribbon trails for the swords, just for fun
	NameValuePairList params;
	params["numberOfChains"] = "2";
	params["maxElements"] = "80";
	mSwordTrail = (RibbonTrail*)m_pSceneMgr->createMovableObject("RibbonTrail", &params);
	mSwordTrail->setMaterialName("Examples/LightRibbonTrail");
	mSwordTrail->setTrailLength(20);
	mSwordTrail->setVisible(false);
	m_pSceneMgr->getRootSceneNode()->attachObject(mSwordTrail);


	for (int i = 0; i < 2; i++)
	{
		mSwordTrail->setInitialColour(i, 1, 0.8, 0);
		mSwordTrail->setColourChange(i, 0.75, 1.25, 1.25, 1.25);
		mSwordTrail->setWidthChange(i, 1);
		mSwordTrail->setInitialWidth(i, 0.5);
	}

	mKeyDirection = Vector3::ZERO;
	mVerticalVelocity = 0;

// part2 setup camera
	
// part3 setup animation
}

//////////////////////////////////////////////////////////////////////////

bool DemoScene1::keyPressed(const OIS::KeyEvent &keyEventRef)
{
	mChara->injectKeyDown(keyEventRef);
//	m_pCameraMan->injectKeyDown(keyEventRef);

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

bool DemoScene1::keyReleased(const OIS::KeyEvent &keyEventRef)
{
	OgreFramework::getSingletonPtr()->keyPressed(keyEventRef);

	//m_pCameraMan->injectKeyUp(keyEventRef);

	mChara->injectKeyUp(keyEventRef);
	return true;
}

//////////////////////////////////////////////////////////////////////////

bool DemoScene1::mouseMoved(const OIS::MouseEvent &evt)
{
	if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseMove(evt)) return true;
	//mChara->injectMouseMove(evt);
	m_pCameraMan->injectMouseMove(evt);

	return true;
}

//|||||||||||||||||||||||||||||||||||||||||||||||

bool DemoScene1::mousePressed(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
{
	if(OgreFramework::getSingletonPtr()->m_pTrayMgr->injectMouseDown(evt, id)) return true;

	m_pCameraMan->injectMouseDown(evt, id);
	//mChara->injectMouseDown(evt, id);

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

bool DemoScene1::mouseReleased(const OIS::MouseEvent &evt, OIS::MouseButtonID id)
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

	m_pCameraMan->injectMouseUp(evt, id);

	return true;
}

void DemoScene1::onLeftPressed(const OIS::MouseEvent &evt)
{

}

//////////////////////////////////////////////////////////////////////////

void DemoScene1::moveCamera()
{
	if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_LSHIFT))
		m_pCamera->moveRelative(mDirection);
	m_pCamera->moveRelative(mDirection / 10);
}

//////////////////////////////////////////////////////////////////////////

void DemoScene1::getInput()
{
	if(m_bSettingsMode == false)
	{
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_A))
			mDirection.x = -m_MoveScale;

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_D))
			mDirection.x = m_MoveScale;

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_W))
			mDirection.z = -m_MoveScale;

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_S))
			mDirection.z = m_MoveScale;

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Q))
			mDirection.y = -m_MoveScale;

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_E))
			mDirection.y = m_MoveScale;

		//camera roll
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_Z))
			m_pCamera->roll(Angle(-m_MoveScale));

		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_X))
			m_pCamera->roll(Angle(m_MoveScale));

		//reset roll
		if(OgreFramework::getSingletonPtr()->m_pKeyboard->isKeyDown(OIS::KC_C))
			m_pCamera->roll(-(m_pCamera->getRealOrientation().getRoll()));
	}
}

//////////////////////////////////////////////////////////////////////////

void DemoScene1::update(double timeSinceLastFrame)
{
	m_FrameEvent.timeSinceLastFrame = timeSinceLastFrame;
	OgreFramework::getSingletonPtr()->m_pTrayMgr->frameRenderingQueued(m_FrameEvent);

	adjustHeight(mChara->getEntity());
	mChara->getEntity()->getParentNode()->translate(Vector3::UNIT_Y * CHAR_HEIGHT);

	mChara->addTime(timeSinceLastFrame);
	
	if(m_bQuit == true)
	{
		popAppState();
		return;
	}

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

	m_MoveScale = mWalkSpeed   * timeSinceLastFrame;
	mRotate  = mRotateSpeed * timeSinceLastFrame;

	mDirection = Vector3::ZERO;


	getInput();
	//moveCamera();
}

//////////////////////////////////////////////////////////////////////////

void DemoScene1::buildGUI()
{
	OgreFramework::getSingletonPtr()->m_pTrayMgr->showFrameStats(OgreBites::TL_BOTTOMLEFT);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->showLogo(OgreBites::TL_BOTTOMRIGHT);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createLabel(OgreBites::TL_TOP, "GameLbl", "DemoScene1", 250);
	OgreFramework::getSingletonPtr()->m_pTrayMgr->showCursor();

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
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createTextBox(OgreBites::TL_RIGHT, "InfoPanel", infoText, 300, 220);

	Ogre::StringVector chatModes;
	chatModes.push_back("Solid mode");
	chatModes.push_back("Wireframe mode");
	chatModes.push_back("Point mode");
	OgreFramework::getSingletonPtr()->m_pTrayMgr->createLongSelectMenu(OgreBites::TL_TOPRIGHT, "ChatModeSelMenu", "ChatMode", 200, 3, chatModes);
}

//////////////////////////////////////////////////////////////////////////

void DemoScene1::itemSelected(OgreBites::SelectMenu* menu)
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
void DemoScene1::defineTerrain(long x, long y)
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
void DemoScene1::initBlendMaps(Ogre::Terrain* terrain)
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
void DemoScene1::configureTerrainDefaults(Ogre::Light* light)
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

void DemoScene1::adjustHeight(Ogre::Entity* ent)
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
