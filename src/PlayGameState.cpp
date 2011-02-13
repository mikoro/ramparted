#include "PlayGameState.h"
#include "ActionManager.h"
#include "BasicCannon.h"
#include "Console.h"
#include "Cursor.h"
#include "GameEngine.h"
#include "LoadProgress.h"
#include "Player.h"
#include "SoundManager.h"
#include "Square.h"
#include "Utility.h"
#include "Wall.h"
#include "Castle.h"
#include "World.h"

#include <ETTerrainInfo.h>
#include <ETTerrainManager.h>
#include <Ogre.h>


PlayGameState::PlayGameState() : GameState("play")
{
	terrainManager = 0;
	cameraFly = false;
}

void PlayGameState::initialize()
{
	LoadProgress loadProgress(6);
	setCameraPosition(CameraTop);
	
	loadProgress.setProgress("Loading ocean");
	oceanLevel = 100.0;
	bool niceWater = Ogre::StringConverter::parseBool(GameEngine::getSingleton().getConfigFile()->getSetting("niceWater", "game"));
	
	Ogre::Plane oceanPlane;
    oceanPlane.normal = Ogre::Vector3::UNIT_Y;
    oceanPlane.d = 20;
    Ogre::MeshManager::getSingleton().createPlane("OceanPlane", "Default", oceanPlane, 8000, 10000, 1, 1, true, 1, 1, 1, Ogre::Vector3::UNIT_Z);
	Ogre::Entity* entity = engine->getSceneManager()->createEntity("OceanPlane", "OceanPlane");
	entity->setMaterialName(niceWater ? "OceanMaterial" : "OceanMaterialLow");
    Ogre::SceneNode* oceanPlaneNode = engine->getSceneManager()->getRootSceneNode()->createChildSceneNode("OceanPlaneNode");
    oceanPlaneNode->attachObject(entity);
    oceanPlaneNode->setPosition(1500, oceanLevel, 4000);
    
	loadProgress.setProgress("Loading terrain");
	terrainManager = new ET::TerrainManager(engine->getSceneManager());
	Ogre::TextureManager::getSingleton().createManual("TerrainLightmap", "Default", Ogre::TEX_TYPE_2D, 512, 512, 1, Ogre::PF_BYTE_RGB);
	terrainManager->setLODErrorMargin(2, engine->getCamera()->getViewport()->getActualHeight());
	Ogre::Image image;
	image.load("TerrainHeightmap1.png", "Default");
	ET::TerrainInfo terrainInfo;
	ET::loadHeightmapFromImage(terrainInfo, image);
	terrainInfo.setExtents(Ogre::AxisAlignedBox(0, 0, 0, 3000, 220, 3000));
	Ogre::Image lightmapImage;
	Ogre::TexturePtr lightmapTexture = Ogre::TextureManager::getSingleton().getByName("TerrainLightmap");
	ET::createTerrainLightmap(terrainInfo, lightmapImage, 512, 512, Ogre::Vector3(1, -1, 1), Ogre::ColourValue::White, Ogre::ColourValue(0.1, 0.1, 0.1));
	lightmapTexture->getBuffer(0, 0)->blitFromMemory(lightmapImage.getPixelBox(0, 0));
	terrainManager->createTerrain(terrainInfo);

	/////////////////
	Ogre::TexturePtr texture = Ogre::TextureManager::getSingleton().createManual(
		"SplattedTerrain", "Default", Ogre::TEX_TYPE_2D, 512, 512, 1, 0, Ogre::PF_BYTE_RGB, Ogre::TU_DYNAMIC_WRITE_ONLY);
			
	Ogre::HardwarePixelBufferSharedPtr pixelBuffer = texture->getBuffer();
	pixelBuffer->lock(Ogre::HardwareBuffer::HBL_DISCARD);
	const Ogre::PixelBox& pixelBox = pixelBuffer->getCurrentLock();
	Ogre::uint8* pDest = static_cast<Ogre::uint8*>(pixelBox.data);

	for(size_t i = 0; i < 512; i++)
	{
		for(size_t j = 0; j < 512; j++)
		{
			*pDest++ = 255;
			*pDest++ = 255;
			*pDest++ = 255;
			*pDest++ = 255;
		}
	}
	pixelBuffer->unlock();

	Ogre::MaterialPtr splattedMaterial = Ogre::MaterialManager::getSingleton().create("SplattedMaterial", "Default");
				
	splattedMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("TerrainTexture1.png");
	splattedMaterial->getTechnique(0)->getPass(0)->setSceneBlending(Ogre::SBT_TRANSPARENT_ALPHA);
	splattedMaterial->getTechnique(0)->getPass(0)->setLightingEnabled(false);
	splattedMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("TerrainLightmap");
	splattedMaterial->getTechnique(0)->getPass(0)->createTextureUnitState("SplattedTerrain");
	terrainManager->setMaterial(splattedMaterial);
	//////////////


	world = new World(engine->getSceneManager()->getRootSceneNode(), 48, 48);
	world->getRoot()->setScale(0.5f, 0.5f, 0.5f);
	world->getRoot()->setPosition(Ogre::Vector3(300, 210, 300));
		
	for(int k = 0; k < 48; k++)
	{
		for(int i = 0; i < 48; i++)
		{
			if(k == 0 || k == 47 || i == 0 || i == 47)
			{
				world->getSquare(i, k)->setBoundary(true);
			}
		}
	}
	
	players.push_back(new Player(world, "Player One"));

	castles.push_back(new Castle(world->getSquare(9, 8)));
	castles.push_back(new Castle(world->getSquare(17, 22)));
	castles.push_back(new Castle(world->getSquare(11, 38)));
	
	loadProgress.setProgress("Adding lights");
	engine->getSceneManager()->setAmbientLight(Ogre::ColourValue(0.05, 0.05, 0.05));
	Ogre::Light* light1 = engine->getSceneManager()->createLight("Light1");
	Ogre::Light* light2 = engine->getSceneManager()->createLight("Light2");
	light1->setType(Ogre::Light::LT_DIRECTIONAL);
	light2->setType(Ogre::Light::LT_DIRECTIONAL);
	light1->setDiffuseColour(Ogre::ColourValue(1, 1, 1));
	light2->setDiffuseColour(Ogre::ColourValue(0.3, 0.3, 0.3));
	light1->setSpecularColour(Ogre::ColourValue(1, 1, 1));
	light2->setSpecularColour(Ogre::ColourValue(0.3, 0.3, 0.3));
	light1->setDirection(1, -1, 1);
	light2->setDirection(-1, -1, -1);

	
	loadProgress.setProgress("Loading sounds");
	engine->getSoundManager()->load("data/sounds/Cannon.ogg", "Cannon", false);
	engine->getSoundManager()->load("data/sounds/Explosion.ogg", "Explosion", false);
	engine->getSoundManager()->load("data/sounds/Splash.ogg", "Splash", false);
	engine->getSoundManager()->load("data/sounds/GroundHit.ogg", "GroundHit", false);
	
	loadProgress.setProgress("Setting keyboard mappings");
	engine->getActionManager()->addAction(0, "rotateCamera", new MouseActionUpdater(OIS::MB_Left, true));
	engine->getActionManager()->addAction(0, "moveLeft", new KeyboardActionUpdater(OIS::KC_A, false));
	engine->getActionManager()->addAction(0, "moveRight", new KeyboardActionUpdater(OIS::KC_D, false));
	engine->getActionManager()->addAction(0, "moveForward", new KeyboardActionUpdater(OIS::KC_W, false));
	engine->getActionManager()->addAction(0, "moveBackward", new KeyboardActionUpdater(OIS::KC_S, false));
	engine->getActionManager()->addAction(0, "accelerateCameraMove", new KeyboardActionUpdater(OIS::KC_LSHIFT, false));
	engine->getActionManager()->addAction(0, "primary", new KeyboardActionUpdater(OIS::KC_COMMA, false));
	engine->getActionManager()->addAction(0, "secondary", new KeyboardActionUpdater(OIS::KC_PERIOD, false));
	
	loadProgress.setProgress("Ready!");
}

void PlayGameState::shutdown()
{
	delete world;
	world = 0;
	
	delete terrainManager;
	terrainManager = 0;
}

void PlayGameState::pause()
{
}

void PlayGameState::resume()
{
}

void PlayGameState::logicTick(double timeStep)
{
	previousStates = currentStates;
	
	if(cameraFly)
	{
		Ogre::Vector3 cameraTranslation(0, 0, 0);
		bool accelerateCameraMove = engine->getActionManager()->getAction(0, "accelerateCameraMove");
		
		const OIS::MouseState& mouseState = engine->getMouse()->getMouseState();
		
		// rotate the camera by using quaternion mathematics
		if(engine->getActionManager()->getAction(0, "rotateCamera"))
		{
			Ogre::Quaternion changeInOrientation1 = Ogre::Quaternion(Ogre::Radian(-mouseState.X.rel * 0.5 * timeStep), Ogre::Vector3::UNIT_Y);
			currentStates.cameraOrientation = changeInOrientation1 * currentStates.cameraOrientation;
			Ogre::Quaternion changeInOrientation2 = Ogre::Quaternion(Ogre::Radian(-mouseState.Y.rel * 0.5 * timeStep), currentStates.cameraOrientation * Ogre::Vector3::UNIT_X);
			currentStates.cameraOrientation = changeInOrientation2 * currentStates.cameraOrientation;
		}
	
		if(engine->getActionManager()->getAction(0, "moveRight"))
			cameraTranslation.x = (accelerateCameraMove ? 1000.0 : 200.0) * timeStep;

		if(engine->getActionManager()->getAction(0, "moveLeft"))
			cameraTranslation.x = -(accelerateCameraMove ? 1000.0 : 200.0) * timeStep;
		
		if(engine->getActionManager()->getAction(0, "moveForward"))
			cameraTranslation.z = -(accelerateCameraMove ? 1000.0 : 200.0) * timeStep;
		
		if(engine->getActionManager()->getAction(0, "moveBackward"))
			cameraTranslation.z = (accelerateCameraMove ? 1000.0 : 200.0) * timeStep;
			
		// move the camera (multiplying the translation vector with the orientation quaternion enables us to move where we're looking at)
		currentStates.cameraPosition += currentStates.cameraOrientation * cameraTranslation;
	}
	else
	{
		if(engine->getActionManager()->getAction(0, "moveRight"))
			players[0]->getCursor()->setAction(Cursor::MoveRight);

		if(engine->getActionManager()->getAction(0, "moveLeft"))
			players[0]->getCursor()->setAction(Cursor::MoveLeft);
		
		if(engine->getActionManager()->getAction(0, "moveForward"))
			players[0]->getCursor()->setAction(Cursor::MoveUp);
		
		if(engine->getActionManager()->getAction(0, "moveBackward"))
			players[0]->getCursor()->setAction(Cursor::MoveDown);

		if(players[0]->getCursor()->getMode() == Cursor::ModeAim)
		{
			if(engine->getActionManager()->getAction(0, "primary"))
				players[0]->requestFire();
			else
				players[0]->allowFiring();
		}
		else if(players[0]->getCursor()->getMode() == Cursor::ModeWalls)
		{
			if(engine->getActionManager()->getAction(0, "secondary"))
				players[0]->getCursor()->setAction(Cursor::Rotate);

			if(engine->getActionManager()->getAction(0, "primary"))
			{
				if(players[0]->getCursor()->placeWalls())
				{
					players[0]->updateWalls();
					players[0]->clearSquares();
					players[0]->clearCannons();
					players[0]->claimSquares();
				}
			}
			else
				players[0]->getCursor()->allowPlacing();
		}

		else if(players[0]->getCursor()->getMode() == Cursor::ModeCannon)
		{
			if(engine->getActionManager()->getAction(0, "primary"))
			{
				players[0]->getCursor()->placeCannon();
			}
			else
				players[0]->getCursor()->allowPlacing();
		}

		players[0]->getCursor()->update(timeStep);
	}
		
	if(engine->getKeyboard()->isKeyDown(OIS::KC_ESCAPE))
		engine->requestShutdown();


	players[0]->updateCannons(timeStep);

}

void PlayGameState::renderTick(double interpolation, double timeStep)
{
	
	players[0]->getCursor()->render(interpolation);
	players[0]->renderCannons(interpolation, timeStep);

	for(std::vector<Castle*>::iterator it = castles.begin(); it != castles.end(); ++it)
	{
		if((*it)->getFlagAnim()->getEnabled())
			(*it)->getFlagAnim()->addTime(timeStep);
	}

	if(cameraFly)
	{
		engine->getCamera()->setOrientation(Ogre::Quaternion::Slerp(interpolation, previousStates.cameraOrientation, currentStates.cameraOrientation, true));
		engine->getCamera()->setPosition(previousStates.cameraPosition * (1.0 - interpolation) + currentStates.cameraPosition * interpolation);
	}
	else
	{
		engine->getCamera()->setOrientation(currentStates.cameraOrientation);
		engine->getCamera()->setPosition(currentStates.cameraPosition);
	}
}

bool PlayGameState::consoleCommandCameraPosition(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		if(args[0] == "top")
			setCameraPosition(CameraTop);
		else if(args[0] == "side")
			setCameraPosition(CameraSide);
		else if(args[0] == "ground")
			setCameraPosition(CameraGround);
		else
			return false;
	}
	else if(args.size() == 3)
	{
		double x = Ogre::StringConverter::parseReal(args[0]);
		double y = Ogre::StringConverter::parseReal(args[1]);
		double z = Ogre::StringConverter::parseReal(args[2]);
		
		previousStates.cameraPosition = currentStates.cameraPosition = Ogre::Vector3(x, y, z);
	}
	else
		return false;
		
	return true;
}

bool PlayGameState::consoleCommandCameraFly(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		cameraFly = Ogre::StringConverter::parseBool(args[0]);
		return true;
	}
	else
		return false;
}

void PlayGameState::setCameraPosition(CameraPosition position)
{
	Ogre::Quaternion newOrientation;
	Ogre::Vector3 newPosition(0, 0, 0);
	
	switch(position)
	{
		case CameraTop:
		{
			newOrientation = Ogre::Quaternion(Ogre::Degree(-90), Ogre::Vector3::UNIT_X) * newOrientation;
			newOrientation = Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y) * newOrientation;
			newPosition = Ogre::Vector3(1500, 3700, 1500);
			
		} break;
		
		case CameraSide:
		{
			newOrientation = Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y) * newOrientation;
			newOrientation = Ogre::Quaternion(Ogre::Degree(45), Ogre::Vector3::UNIT_X) * newOrientation;
			newPosition = Ogre::Vector3(1500, 2500, -1100);
			
		} break;
		
		case CameraGround:
		{
			newOrientation = Ogre::Quaternion(Ogre::Degree(180), Ogre::Vector3::UNIT_Y) * newOrientation;
			newPosition = Ogre::Vector3(1500, 230, 0);
			
		} break;
	}
	
	previousStates.cameraOrientation = currentStates.cameraOrientation = newOrientation;
	previousStates.cameraPosition = currentStates.cameraPosition = newPosition;
}

void PlayGameState::keyPressed(const OIS::KeyEvent& event)
{
	switch(event.key)
	{
		case OIS::KC_F2:
		{
			if(players[0]->getCursor()->getMode() == Cursor::ModeAim)
			{
				players[0]->getCursor()->setMode(Cursor::ModeWalls);
			}
			else if (players[0]->getCursor()->getMode() == Cursor::ModeWalls)
			{
				players[0]->getCursor()->setMode(Cursor::ModeCannon);
			}
			else
			{
				players[0]->getCursor()->setMode(Cursor::ModeAim);
			}

		} break;

		default: break;
	}
}
