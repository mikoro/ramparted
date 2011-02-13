#ifndef RAMPARTED_GAMEENGINE
#define RAMPARTED_GAMEENGINE

#include <OGRE/OgreSingleton.h>
#include <OGRE/OgreWindowEventUtilities.h>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>

class ActionManager;
class Console;
class FpsCounter;
class GameState;
class InterpolationAffectorFactory;
class SoundManager;

namespace Ogre
{
	class Camera;
	class ConfigFile;
	class Log;
	class RenderWindow;
	class Root;
	class SceneManager;
}

/// Backbone of the program.
class GameEngine : public OIS::KeyListener, public OIS::MouseListener, public Ogre::WindowEventListener, public Ogre::Singleton<GameEngine>
{
public:

	/// Construct a new instance.
	GameEngine();
	
	/// Initialize the engine.
	/// This function must be called before doing anything else.
	void initialize();
	
	/// Start the mainloop.
	/// Will run as long as there are states available, so remember to insert some before calling this function.
	void mainloop();
	
	/// Release all resources.
	/// After this call you should not do anything with the object.
	void shutdown();
	
	/// Push new game state to the stack.
	/// Active state (if any) will be paused and pushed down.
	/// @param newState State to be added.
	void pushState(GameState* newState);
	
	/// Change active state.
	/// Active state (if any) will be shutdown.
	/// @param newState State to be added.
	void changeState(GameState* newState);
	
	/// Pop out the active state.
	/// Active state (if any) will be shutdown and previous state (if any) will be resumed.
	void popState();
	
	/// Request engine to shutdown.
	/// Shutdown will not be immediate - it will be done as soon as possible (to allow clean shutdown).
	void requestShutdown();
	
	/// Do one extra render cycle.
	/// Only call this when the normal rendering cycle fails to show changes on the screen (e.g. when changing states).
	void doExtraRendering();
	
	bool consoleCommandFps(std::vector<std::string> args); 					///< Implementation for the console command "fps".
	bool consoleCommandWireframe(std::vector<std::string> args); 			///< Implementation for the console command "wireframe".
	bool consoleCommandSkybox(std::vector<std::string> args); 				///< Implementation for the console command "skybox".
	bool consoleCommandCameraFOV(std::vector<std::string> args); 			///< Implementation for the console command "cameraFOV".
	bool consoleCommandCameraPanel(std::vector<std::string> args); 			///< Implementation for the console command "cameraPanel".
	bool consoleCommandCameraAspect(std::vector<std::string> args); 		///< Implementation for the console command "cameraAspect".
	
	static GameEngine& getSingleton();				///< Get the singleton as reference.
    static GameEngine* getSingletonPtr();			///< Get the singleton as pointer.
	Ogre::Root* getRoot() const; 					///< Get the Ogre root object.
	Ogre::RenderWindow* getWindow() const; 			///< Get the Ogre render window object.
	Ogre::SceneManager* getSceneManager() const; 	///< Get the default Ogre scene manager.
	Ogre::Camera* getCamera() const; 				///< Get the default Ogre camera object.
	Ogre::Log* getLog() const; 						///< Get the default Ogre log.
	Ogre::ConfigFile* getConfigFile() const; 		///< Get the default Ogre configuration file.
	OIS::Keyboard* getKeyboard() const; 			///< Get the OIS keyboard object.
	OIS::Mouse* getMouse() const; 					///< Get the OIS mouse object.
	SoundManager* getSoundManager() const; 			///< Get the sound manager object.
	ActionManager* getActionManager() const; 		///< Get the action manager object.
	Console* getConsole() const; 					///< Get the console object.
	GameState* getState(std::string name) const; 	///< Get a game state by name.
	unsigned int getWindowWidth() const;			///< Get the render window width.
	unsigned int getWindowHeight() const;			///< Get the render window height.

private:
	
	bool keyPressed(const OIS::KeyEvent& event); 									///< Callback function, fires when a keyboard key has been pressed.
	bool keyReleased(const OIS::KeyEvent& event); 									///< Callback function, fires when a keyboard key has been released.
	bool mousePressed(const OIS::MouseEvent& event, OIS::MouseButtonID button); 	///< Callback function, fires when a mouse button has been pressed.
	bool mouseReleased(const OIS::MouseEvent& event, OIS::MouseButtonID button); 	///< Callback function, fires when a mouse button has been released.
	bool mouseMoved(const OIS::MouseEvent& event); 									///< Callback function, fires when the mouse has been moved.
	void windowClosed(Ogre::RenderWindow* renderWindow); 							///< Callback function, fires when the main window has been closed.
	void windowFocusChange(Ogre::RenderWindow* renderWindow); 						///< Callback function, fires when the main window has lost/regained the focus.
	void windowMoved(Ogre::RenderWindow* renderWindow); 							///< Callback function, fires when the main window has moved.
	void windowResized(Ogre::RenderWindow* renderWindow); 							///< Callback function, fires when the main window has changed its size.
	
	std::vector<GameState*> states;
	Ogre::Root* root;
	Ogre::RenderWindow* window;
	Ogre::SceneManager* sceneManager;
	Ogre::Camera* camera;
	Ogre::Viewport* viewport;
	Ogre::Log* log;
	Ogre::ConfigFile* configFile;
	Ogre::Overlay* fpsOverlay;
	Ogre::Overlay* cameraOverlay;
	OIS::InputManager* inputManager;
	OIS::Keyboard* keyboard;
	OIS::Mouse* mouse;
	SoundManager* soundManager;
	ActionManager* actionManager;
	Console* console;
	FpsCounter* renderFps;
	FpsCounter* logicFps;
	InterpolationAffectorFactory* interpFactory;
	bool shutdownRequested;
	double timeStep;
	unsigned int windowWidth;
	unsigned int windowHeight;
};

#endif
