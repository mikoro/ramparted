#ifndef RAMPARTED_PLAYGAMESTATE
#define RAMPARTED_PLAYGAMESTATE

#include "GameState.h"

#include <OGRE/OgreVector3.h>
#include <OGRE/OgreQuaternion.h>

class Castle;
class Player;
class World;

namespace ET
{
	class TerrainManager;
}

/// State for managing the playing itself.
class PlayGameState : public GameState
{
public:

	PlayGameState();
	
	void initialize();
	void shutdown();
	
	void pause();
	void resume();
	
	void logicTick(double timeStep);
	void renderTick(double interpolation, double timeStep);
	
	bool consoleCommandCameraPosition(std::vector<std::string> args); 	///< Implementation for the console command "cameraPosition".
	bool consoleCommandCameraFly(std::vector<std::string> args); 		///< Implementation for the console command "cameraFly".

	ET::TerrainManager* getTerrainManager() { return terrainManager; }
	Ogre::Real getOceanLevel() { return oceanLevel; }

private:
	
	enum CameraPosition { CameraTop, CameraSide, CameraGround };
	void setCameraPosition(CameraPosition position);

	void keyPressed(const OIS::KeyEvent& event);
	
	struct States
	{
		Ogre::Quaternion cameraOrientation;
		Ogre::Vector3 cameraPosition;
	};
	
	States previousStates;
	States currentStates;
	ET::TerrainManager* terrainManager;
	World* world;
	std::vector<Player*> players;
	std::vector<Castle*> castles;
	bool cameraFly;
	Ogre::Real oceanLevel;
};

#endif
