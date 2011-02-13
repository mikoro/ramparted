#ifndef RAMPARTED_MENUGAMESTATE
#define RAMPARTED_MENUGAMESTATE

#include "GameState.h"

/// State for managing the menu.
class MenuGameState : public GameState
{
public:

	MenuGameState();
	
	void initialize();
	void shutdown();
	
	void pause();
	void resume();
	
	void logicTick(double timeStep);
	void renderTick(double interpolation, double timeStep);
	
private:
		
};

#endif
