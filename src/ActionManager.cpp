#include "ActionManager.h"
#include "Console.h"
#include "GameEngine.h"

KeyboardActionUpdater::KeyboardActionUpdater(OIS::KeyCode keyCode, bool enabledWithConsole)
{
	this->keyCode = keyCode;
	this->enabledWithConsole = enabledWithConsole;
}

MouseActionUpdater::MouseActionUpdater(OIS::MouseButtonID mouseButton, bool enabledWithConsole)
{
	this->mouseButton = mouseButton;
	this->enabledWithConsole = enabledWithConsole;
}

bool KeyboardActionUpdater::getActionState()
{
	if(!enabledWithConsole && GameEngine::getSingleton().getConsole()->isEnabled())
		return false;
	else
		return GameEngine::getSingletonPtr()->getKeyboard()->isKeyDown(keyCode);
}

bool MouseActionUpdater::getActionState()
{
	if(!enabledWithConsole && GameEngine::getSingleton().getConsole()->isEnabled())
		return false;
	else
		return GameEngine::getSingletonPtr()->getMouse()->getMouseState().buttonDown(mouseButton);
}

ActionManager::ActionManager()
{
	actionContainerMaps = std::vector<actionContainerMap>(4); // just four players maximum at the moment
}

void ActionManager::addAction(int player, std::string identifier, ActionUpdater* updater)
{
	// remove existing identical action
	if(actionContainerMaps[player].find(identifier) != actionContainerMaps[player].end())
	{
		delete actionContainerMaps[player][identifier];
		actionContainerMaps[player].erase(identifier);
	}
	
	// add action to correct player
	actionContainerMaps[player][identifier] = new ActionContainer(updater);
}

bool ActionManager::getAction(int player, std::string identifier)
{
	return actionContainerMaps[player][identifier]->state;
}
	
void ActionManager::update()
{
	// go through every player and their every action and update its state
	for(std::vector<actionContainerMap>::iterator map = actionContainerMaps.begin(); map != actionContainerMaps.end(); ++map)
		for(actionContainerMap::iterator it = map->begin(); it != map->end(); ++it)
			it->second->state = it->second->updater->getActionState();
}

void ActionManager::shutdown()
{
	for(std::vector<actionContainerMap>::iterator map = actionContainerMaps.begin(); map != actionContainerMaps.end(); ++map)
		for(actionContainerMap::iterator it = map->begin(); it != map->end(); ++it)
			delete it->second;
}
