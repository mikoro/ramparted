// this file should be included in the Main.cpp
#include "Console.h"
#include "PlayGameState.h"

bool quit(std::vector<std::string>)
{
	GameEngine::getSingleton().requestShutdown();
	return true;
}

bool help(std::vector<std::string> args)
{
	return GameEngine::getSingleton().getConsole()->consoleCommandHelp(args);
}

bool fps(std::vector<std::string> args)
{
	return GameEngine::getSingleton().consoleCommandFps(args);
}

bool wireframe(std::vector<std::string> args)
{
	return GameEngine::getSingleton().consoleCommandWireframe(args);
}

bool skybox(std::vector<std::string> args)
{
	return GameEngine::getSingleton().consoleCommandSkybox(args);
}

bool arguments(std::vector<std::string> args)
{
	return GameEngine::getSingleton().getConsole()->consoleCommandArguments(args);
}

bool consolePrefix(std::vector<std::string> args)
{
	return GameEngine::getSingleton().getConsole()->consoleCommandConsolePrefix(args);
}

bool consoleSpeed(std::vector<std::string> args)
{
	return GameEngine::getSingleton().getConsole()->consoleCommandConsoleSpeed(args);
}

bool cameraFOV(std::vector<std::string> args)
{
	return GameEngine::getSingleton().consoleCommandCameraFOV(args);
}

bool cameraPanel(std::vector<std::string> args)
{
	return GameEngine::getSingleton().consoleCommandCameraPanel(args);
}

bool cameraAspect(std::vector<std::string> args)
{
	return GameEngine::getSingleton().consoleCommandCameraAspect(args);
}

bool cameraPosition(std::vector<std::string> args)
{
	PlayGameState* state = (PlayGameState*)GameEngine::getSingleton().getState("play");
	
	if(state)
		return state->consoleCommandCameraPosition(args);
	else
		return true;
}

bool cameraFly(std::vector<std::string> args)
{
	PlayGameState* state = (PlayGameState*)GameEngine::getSingleton().getState("play");

	if(state)
		return state->consoleCommandCameraFly(args);
	else
		return true;
}
