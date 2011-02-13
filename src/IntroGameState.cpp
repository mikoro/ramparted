#include "IntroGameState.h"
#include "Console.h"
#include "GameEngine.h"
#include "PlayGameState.h"
#include "SoundManager.h"

#include <Ogre.h>

IntroGameState::IntroGameState() : GameState("intro")
{
	logoOverlay = 0;
}

void IntroGameState::initialize()
{
	logoOverlay = Ogre::OverlayManager::getSingletonPtr()->getByName("LogoOverlay");
	logoOverlay->show();
}

void IntroGameState::shutdown()
{
}

void IntroGameState::pause()
{
}

void IntroGameState::resume()
{
}

void IntroGameState::logicTick(double)
{
	if(engine->getKeyboard()->isKeyDown(OIS::KC_ESCAPE))
		engine->requestShutdown();
	
	if((!engine->getConsole()->isEnabled() && (engine->getKeyboard()->isKeyDown(OIS::KC_SPACE) || engine->getKeyboard()->isKeyDown(OIS::KC_RETURN))) ||
	   engine->getMouse()->getMouseState().buttonDown(OIS::MB_Left))
	{
		logoOverlay->hide();
		engine->changeState(new PlayGameState());
	}
}

void IntroGameState::renderTick(double, double)
{
}
