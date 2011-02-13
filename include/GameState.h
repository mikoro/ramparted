#ifndef RAMPARTED_GAMESTATE
#define RAMPARTED_GAMESTATE

#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>
#include <string>

class GameEngine;

/// Interface for game states.
class GameState
{
public:

	/// Destroy the instance.
	virtual ~GameState() {};

	/// Initialize the state.
	/// Callback function - fires only once, when the state is inserted.
	virtual void initialize() = 0;

	/// Shutdown the state.
	/// Callback function - fires only once, when the state is removed. All resources should be released here.
	virtual void shutdown() = 0;

	/// Pause the state.
	/// Callback funtion - fires when the state gets pushed away by an another state.
	virtual void pause() = 0;

	/// Resume the state.
	/// Callback function - fires when the state becomes the active state again.
	virtual void resume() = 0;

	/// Do the logic.
	/// Callback function - fires fixed times per second (adjustable in the engine)
	/// @param timeStep The time elapsed from the previous call in seconds. This should be used every time you integrate something.
	virtual void logicTick(double timeStep) = 0;

	/// Do the graphics.
	/// Callback function - fires as many times per second as possible.
	/// @param interpolation A value between [0,1] indicating the transition phase from previous logicTick to current logicTick. This should be used smooth the paths of the renderable objects.
	/// @param timeStep The time elapsed from the previous call in seconds. This should only be used for external animations, for example mesh animations.
	virtual void renderTick(double interpolation, double timeStep) = 0;

	virtual void keyPressed(const OIS::KeyEvent&) {}; 								///< Callback function, fires when a keyboard key has been pressed.
	virtual void keyReleased(const OIS::KeyEvent&) {}; 								///< Callback function, fires when a keyboard key has been released.
	virtual void mousePressed(const OIS::MouseEvent&, OIS::MouseButtonID) {}; 		///< Callback function, fires when a mouse button has been pressed.
	virtual void mouseReleased(const OIS::MouseEvent&, OIS::MouseButtonID) {}; 		///< Callback function, fires when a mouse button has been released.
	virtual void mouseMoved(const OIS::MouseEvent&) {}; 							///< Callback function, fires when the mouse has been moved.
	std::string getName() const { return name; } 									///< Get the name of the state.

protected:

	/// Construct a new instance.
	/// @param name Name of the state.
	GameState(std::string name);

	GameEngine* engine; ///< Pointer to the game engine this state belongs to.

private:

	std::string name; ///< Name of the state, to be used in logging etc.
};

#endif
