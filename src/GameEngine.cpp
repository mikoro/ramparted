#include "GameEngine.h"
#include "ActionManager.h"
#include "Console.h"
#include "FpsCounter.h"
#include "GameState.h"
#include "InterpolationAffectorFactory.h"
#include "SoundManager.h"

#include <iostream>
#include <Ogre.h>
#include <OIS/OISInputManager.h>
#include <stdexcept>
#include <string>

template<> GameEngine* Ogre::Singleton<GameEngine>::ms_Singleton = 0;

GameEngine::GameEngine()
{
	root = 0;
	window = 0;
	sceneManager = 0;
	camera = 0;
	viewport = 0;
	configFile = 0;
	fpsOverlay = 0;
	cameraOverlay = 0;
	log = 0;
	inputManager = 0;
	keyboard = 0;
	mouse = 0;
	soundManager = 0;
	actionManager = 0;
	console = 0;
	renderFps = 0;
	logicFps = 0;
	interpFactory = 0;
	shutdownRequested = false;
	timeStep = 0;
	windowWidth = 0;
	windowHeight = 0;
}

void GameEngine::initialize()
{
	configFile = new Ogre::ConfigFile();
	configFile->load("ramparted.ini", "=", false);
	int width = Ogre::StringConverter::parseInt(configFile->getSetting("width", "video"));
	int height = Ogre::StringConverter::parseInt(configFile->getSetting("height", "video"));
	bool fullscreen = Ogre::StringConverter::parseBool(configFile->getSetting("fullscreen", "video"));
	bool consoleOutput = Ogre::StringConverter::parseBool(configFile->getSetting("consoleOutput", "logging"));
	int loggingLevel = Ogre::StringConverter::parseInt(configFile->getSetting("level", "logging"));
	std::string pluginsPath = configFile->getSetting("pluginsPath", "ogre");
	timeStep = Ogre::StringConverter::parseReal(configFile->getSetting("timeStep", "engine"));
	double consoleSpeed = Ogre::StringConverter::parseReal(configFile->getSetting("speed", "console"));
	bool showFps = Ogre::StringConverter::parseBool(configFile->getSetting("fps", "engine"));
	bool showCameraDetail = Ogre::StringConverter::parseBool(configFile->getSetting("cameraDetail", "engine"));
	bool grabInput = Ogre::StringConverter::parseBool(configFile->getSetting("grabInput", "engine"));
	bool enableSound = Ogre::StringConverter::parseBool(configFile->getSetting("sound", "engine"));
	
	new Ogre::LogManager();
	log = Ogre::LogManager::getSingleton().createLog("ramparted.log", true, consoleOutput, false);
	
	Ogre::LoggingLevel ogreLoggingLevel;
	
	switch(loggingLevel)
	{
		case 0: ogreLoggingLevel = Ogre::LL_BOREME; break;
		case 1: ogreLoggingLevel = Ogre::LL_NORMAL; break;
		case 2: ogreLoggingLevel = Ogre::LL_LOW; break;
		default: ogreLoggingLevel = Ogre::LL_NORMAL;
	}
	
	Ogre::LogManager::getSingleton().setLogDetail(ogreLoggingLevel);
	
	console = new Console(consoleSpeed, ">> ");
	log->addListener(console);
	
	root = new Ogre::Root("", "");
	
#ifdef _DEBUG
	root->loadPlugin(pluginsPath + "RenderSystem_GL_d");
	root->loadPlugin(pluginsPath + "Plugin_OctreeSceneManager_d");
	root->loadPlugin(pluginsPath + "Plugin_CgProgramManager_d");
	root->loadPlugin(pluginsPath + "Plugin_ParticleFX_d");
#else
	root->loadPlugin(pluginsPath + "RenderSystem_GL");
	root->loadPlugin(pluginsPath + "Plugin_OctreeSceneManager");
	root->loadPlugin(pluginsPath + "Plugin_CgProgramManager");
	root->loadPlugin(pluginsPath + "Plugin_ParticleFX");
#endif
	
	root->setRenderSystem(root->getRenderSystemByName("OpenGL Rendering Subsystem"));
	root->initialise(false);

	interpFactory = new InterpolationAffectorFactory();
	Ogre::ParticleSystemManager::getSingleton().addAffectorFactory(interpFactory);
	
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/fonts", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/materials", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/meshes", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/miscellaneous", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/overlays", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/particles", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/shaders", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/sounds", "FileSystem", "Default");
	Ogre::ResourceGroupManager::getSingleton().addResourceLocation("data/textures", "FileSystem", "Default");
	
	window = root->createRenderWindow("Ramparted", width, height, fullscreen);
	sceneManager = root->createSceneManager("OctreeSceneManager", "Ramparted");
	
	Ogre::ResourceGroupManager::getSingleton().initialiseAllResourceGroups();
	Ogre::WindowEventUtilities::addWindowEventListener(window, this);
	
	console->initialize();
	
	camera = sceneManager->createCamera("DefaultCamera");
	camera->setNearClipDistance(5);
	viewport = window->addViewport(camera);
	viewport->setBackgroundColour(Ogre::ColourValue(0, 0, 0));
	
	log->logMessage("Initializing input system", Ogre::LML_NORMAL);
	
	OIS::ParamList paramList;
	size_t windowHandle = 0;
	window->getCustomAttribute("WINDOW", &windowHandle);
	paramList.insert(std::make_pair("WINDOW", Ogre::StringConverter::toString(windowHandle)));

#ifdef WIN32
	paramList.insert(std::make_pair("w32_mouse", (fullscreen || grabInput) ? "DISCL_EXCLUSIVE" : "DISCL_NONEXCLUSIVE"));
	paramList.insert(std::make_pair("w32_mouse", "DISCL_FOREGROUND"));
	paramList.insert(std::make_pair("w32_keyboard", (fullscreen || grabInput) ? "DISCL_EXCLUSIVE" : "DISCL_NONEXCLUSIVE"));
	paramList.insert(std::make_pair("w32_keyboard", "DISCL_FOREGROUND"));
#else
	paramList.insert(std::make_pair("XAutoRepeatOn", "true"));
	paramList.insert(std::make_pair("x11_keyboard_grab", (fullscreen || grabInput) ? "true" : "false"));
	paramList.insert(std::make_pair("x11_mouse_grab", (fullscreen || grabInput) ? "true" : "false"));
	paramList.insert(std::make_pair("x11_mouse_hide", (fullscreen || grabInput) ? "true" : "false"));
#endif
	
	inputManager = OIS::InputManager::createInputSystem(paramList);
	keyboard = static_cast<OIS::Keyboard*>(inputManager->createInputObject(OIS::OISKeyboard, true));
	mouse = static_cast<OIS::Mouse*>(inputManager->createInputObject(OIS::OISMouse, true));
	
	keyboard->setEventCallback(this);
	mouse->setEventCallback(this);
	
	windowResized(window);
	
	soundManager = new SoundManager();
	soundManager->initialize();
	soundManager->setMuted(!enableSound);
	
	actionManager = new ActionManager();

	renderFps = new FpsCounter();
	logicFps = new FpsCounter();
	
	fpsOverlay = Ogre::OverlayManager::getSingletonPtr()->getByName("FpsOverlay");
	cameraOverlay = Ogre::OverlayManager::getSingletonPtr()->getByName("CameraOverlay");
	
	if(showFps)
		fpsOverlay->show();
	
	if(showCameraDetail)
		cameraOverlay->show();
}

void GameEngine::mainloop()
{
	log->logMessage("Entered the mainloop", Ogre::LML_NORMAL);
	
	Ogre::Timer timer;
	double oldTime = (double)timer.getMicroseconds() / 1000000.0;
	double accumulator = 0;
	
	while(!shutdownRequested)
	{
		double newTime = (double)timer.getMicroseconds() / 1000000.0;
		double deltaTime = newTime - oldTime;
		accumulator += deltaTime;
		oldTime = newTime;

		while(accumulator >= timeStep && !shutdownRequested)
		{
			accumulator -= timeStep;
			keyboard->capture();
			mouse->capture();
			actionManager->update();
			soundManager->update();
			logicFps->tick();
			console->logicTick(timeStep);
			Ogre::WindowEventUtilities::messagePump();
			
			if(fpsOverlay->isVisible())
			{
				fpsOverlay->getChild("FpsPanel")->getChild("RenderFpsTextArea")->setCaption("Render: " + renderFps->getFps() + " Hz");
				fpsOverlay->getChild("FpsPanel")->getChild("LogicFpsTextArea")->setCaption("Logic:  " + logicFps->getFps() + " Hz");
			}
			
			if(cameraOverlay->isVisible())
			{
				cameraOverlay->getChild("CameraPanel")->getChild("CameraPositionTextArea")->setCaption("Position: " + Ogre::StringConverter::toString(camera->getPosition()));
				cameraOverlay->getChild("CameraPanel")->getChild("CameraOrientationTextArea")->setCaption("Orientation: " + Ogre::StringConverter::toString(camera->getOrientation()));
				cameraOverlay->getChild("CameraPanel")->getChild("CameraAspectTextArea")->setCaption("Aspect ratio: " + Ogre::StringConverter::toString(camera->getAspectRatio()));
				cameraOverlay->getChild("CameraPanel")->getChild("CameraFOVTextArea")->setCaption("Field of view: " + Ogre::StringConverter::toString(camera->getFOVy()));
			}
			
			if(!shutdownRequested)
				states.back()->logicTick(timeStep);
		}
		
		double interpolation = accumulator / timeStep;
		states.back()->renderTick(interpolation, deltaTime);
		console->renderTick(interpolation, deltaTime);
		root->renderOneFrame();
		renderFps->tick();
	}
}

void GameEngine::shutdown()
{
	if(log)
		log->logMessage("Removing game states", Ogre::LML_NORMAL);
	
	while(!states.empty())
	{
		states.back()->shutdown();
		delete states.back();
		states.pop_back();
	}
	
	delete logicFps;
	delete renderFps;
	logicFps = 0;
	renderFps = 0;
	
	if(actionManager)
		actionManager->shutdown();
	
	delete actionManager;
	actionManager = 0;
	
	if(soundManager)
		soundManager->shutdown();
	
	if(window)
	{
		windowClosed(window);
		Ogre::WindowEventUtilities::removeWindowEventListener(window, this);
	}
	
	camera = 0;
	sceneManager = 0;
	window = 0;
	
	if(log)
		log->removeListener(console);
	
	if(console)
		console->shutdown();
	
	delete console;
	console = 0;
	
	delete configFile;
	configFile = 0;
	
	delete root;
	root = 0;

	delete interpFactory;
	interpFactory = 0;
	
	log = 0;
}

void GameEngine::pushState(GameState* newState)
{
	log->logMessage("Pushing a new state: " + newState->getName(), Ogre::LML_NORMAL);
	
	if(!states.empty())
		states.back()->pause();
	
	states.push_back(newState);
	states.back()->initialize();
}

void GameEngine::changeState(GameState* newState)
{
	log->logMessage("Changing the state: " + newState->getName(), Ogre::LML_NORMAL);
	
	if(!states.empty())
	{
		states.back()->shutdown();
		delete states.back();
		states.pop_back();
	}
	
	states.push_back(newState);
	states.back()->initialize();
}

void GameEngine::popState()
{
	if(!states.empty())
	{
		log->logMessage("Popping the state: " + states.back()->getName(), Ogre::LML_NORMAL);
		
		states.back()->shutdown();
		delete states.back();
		states.pop_back();
		
		if(!states.empty())
			states.back()->resume();
	}
}

void GameEngine::requestShutdown()
{
	if(log)
		log->logMessage("Shutdown requested", Ogre::LML_NORMAL);
	
	shutdownRequested = true;
}

void GameEngine::doExtraRendering()
{
	root->renderOneFrame();
}

bool GameEngine::consoleCommandFps(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		if(args[0] == "1")
			fpsOverlay->show();
		else if(args[0] == "0")
			fpsOverlay->hide();
		else
			return false;
	}
	else
		return false;
	
	return true;
}

bool GameEngine::consoleCommandWireframe(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		if(args[0] == "1")
			camera->setPolygonMode(Ogre::PM_WIREFRAME);
		else if(args[0] == "0")
			camera->setPolygonMode(Ogre::PM_SOLID);
		else
			return false;
	}
	else
		return false;
	
	return true;
}

bool GameEngine::consoleCommandSkybox(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		bool enabled = Ogre::StringConverter::parseBool(args[0]);
		sceneManager->setSkyBox(enabled, "SkyMaterial", 10000, true);
		return true;
	}
	else
		return false;
}

bool GameEngine::consoleCommandCameraFOV(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		camera->setFOVy(Ogre::Degree(Ogre::StringConverter::parseInt(args[0])));
		return true;
	}
	else
		return false;
}

bool GameEngine::consoleCommandCameraPanel(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		if(args[0] == "1")
			cameraOverlay->show();
		else if(args[0] == "0")
			cameraOverlay->hide();
		else
			return false;
	}
	else
		return false;
	
	return true;
}

bool GameEngine::consoleCommandCameraAspect(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		if(args[0] == "orig")
			camera->setAspectRatio(Ogre::Real(viewport->getActualWidth()) / Ogre::Real(viewport->getActualHeight()));
		else
			camera->setAspectRatio(Ogre::StringConverter::parseReal(args[0]));
		
		return true;
	}
	else
		return false;
}

GameEngine& GameEngine::getSingleton(void) { assert(ms_Singleton); return *ms_Singleton; }
GameEngine* GameEngine::getSingletonPtr(void) { return ms_Singleton; }
Ogre::Root* GameEngine::getRoot() const { return root; }
Ogre::RenderWindow* GameEngine::getWindow() const { return window; }
Ogre::SceneManager* GameEngine::getSceneManager() const { return sceneManager; }
Ogre::Camera* GameEngine::getCamera() const { return camera; }
Ogre::Log* GameEngine::getLog() const { return log; }
Ogre::ConfigFile* GameEngine::getConfigFile() const { return configFile; }
OIS::Keyboard* GameEngine::getKeyboard() const { return keyboard; }
OIS::Mouse* GameEngine::getMouse() const { return mouse; }
SoundManager* GameEngine::getSoundManager() const { return soundManager; }
ActionManager* GameEngine::getActionManager() const { return actionManager; }
Console* GameEngine::getConsole() const { return console; }
unsigned int GameEngine::getWindowWidth() const { return windowWidth; }
unsigned int GameEngine::getWindowHeight() const { return windowHeight; }

GameState* GameEngine::getState(std::string name) const
{
	for(unsigned int i=0; i<states.size(); ++i)
	{
		if(states[i]->getName() == name)
			return states[i];
	}
	
	return NULL;
}

bool GameEngine::keyPressed(const OIS::KeyEvent& event)
{
	if(!states.empty())
		states.back()->keyPressed(event);
	
	console->keyPressed(event);
	
	if(event.key == OIS::KC_F12)
	{
		log->logMessage("Taking a screenshot...");
		window->writeContentsToTimestampedFile("screenshot", ".jpg");
	}
	
	return true;
}

bool GameEngine::keyReleased(const OIS::KeyEvent& event)
{
	if(!states.empty())
		states.back()->keyReleased(event);
	
	return true;
}

bool GameEngine::mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID button)
{
	if(!states.empty())
		states.back()->mousePressed(event, button);
	
	return true;
}

bool GameEngine::mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID button)
{
	if(!states.empty())
		states.back()->mouseReleased(event, button);
	
	return true;
}

bool GameEngine::mouseMoved(const OIS::MouseEvent& event)
{
	if(!states.empty())
		states.back()->mouseMoved(event);
	
	return true;
}

void GameEngine::windowClosed(Ogre::RenderWindow*)
{
	if(inputManager)
	{
		inputManager->destroyInputObject(mouse);
		inputManager->destroyInputObject(keyboard);
		OIS::InputManager::destroyInputSystem(inputManager);
		
		mouse = 0;
		keyboard = 0;
		inputManager = 0;
	}
	
	requestShutdown();
}

void GameEngine::windowFocusChange(Ogre::RenderWindow*) {}
void GameEngine::windowMoved(Ogre::RenderWindow*) {}

void GameEngine::windowResized(Ogre::RenderWindow* renderWindow)
{
	int left, top;
	unsigned int depth;
	renderWindow->getMetrics(windowWidth, windowHeight, depth, left, top);
	mouse->getMouseState().width = windowWidth;
	mouse->getMouseState().height = windowHeight;
	console->setWindowWidth(windowWidth);
	console->setWindowHeight(windowHeight);
	camera->setAspectRatio(windowWidth / (double)windowHeight);
}
