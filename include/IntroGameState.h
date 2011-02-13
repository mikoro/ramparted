#ifndef RAMPART_INTROGAMESTATE
#define RAMPART_INTROGAMESTATE

#include "GameState.h"

namespace Ogre
{
	class Overlay;
}

/// State for managing the intro.
class IntroGameState : public GameState
{
public:

	IntroGameState();
	
	void initialize();
	void shutdown();
	
	void pause();
	void resume();
	
	void logicTick(double timeStep);
	void renderTick(double interpolation, double timeStep);
	
private:

	Ogre::Overlay* logoOverlay;
};

#endif
