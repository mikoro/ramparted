#include "GameState.h"
#include "GameEngine.h"

GameState::GameState(std::string name)
{
	this->name = name;
	engine = GameEngine::getSingletonPtr();
}
