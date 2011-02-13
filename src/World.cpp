#include "World.h"
#include "GameEngine.h"
#include "ParticlePool.h"
#include "Player.h"
#include "Square.h"
#include "Utility.h"

#include <Ogre.h>

World::World(Ogre::SceneNode* origin, int x, int z)
{
	xSize = x;
	zSize = z;
	
	pools["ExplosionSmoke"] = new ParticlePool("ExplosionSmokeParticle");
	pools["ExplosionSolid"] = new ParticlePool("ExplosionParticle");
	pools["CannonBallHit"] = new ParticlePool("CannonBallHitParticle");
	pools["WaterSplash"] = new ParticlePool("WaterSplashParticle");
	
	root = origin->createChildSceneNode(generateId());
		
	//create the square matrix
	squares = std::vector<std::vector<Square*> >(zSize);
	for(int k = 0; k < zSize; k++)
	{
		squares[k] = std::vector<Square*>(xSize);
	}
	
	//fill the square matrix with squares
	Square* tempSquare;
	for(int k = 0; k < zSize; k++)
	{
		for(int i = 0; i < xSize; i++)
		{
			tempSquare = new Square(this, i, k);
			squares[k][i] = tempSquare;
		}
	}
}


Square* World::getSquare(int x, int z)
{
	if(x < 0 || x >= xSize || z < 0 || z >= zSize) return NULL;
	return squares[z][x];
}


Square* World::getSquare(Ogre::Vector3 pos)
{
	int xCoord = (int)(pos.x / 100.0);
	int zCoord = (int)(pos.z / 100.0);
	
	if(pos.x / 100 - (double)xCoord >= 0.5) xCoord++;
	if(pos.z / 100 - (double)zCoord >= 0.5) zCoord++;
	
	if(xCoord < 0 || xCoord > xSize - 1 || zCoord < 0 || zCoord > zSize - 1)
	{
		return NULL;
	}
	
	return this->getSquare(xCoord, zCoord);
}


Square* World::getSquareO(Ogre::Vector3 pos)
{
	Ogre::Vector3 origin = root->getWorldPosition();
	Ogre::Vector3 scale = root->getScale();
	Ogre::Vector3 scaledPos = (1/scale) * (pos - origin);

	int xCoord = (int)(scaledPos.x / 100.0);
	int zCoord = (int)(scaledPos.z / 100.0);

	if((scaledPos.x / 100.0) - (double)xCoord >= 0.5) xCoord++;
	if((scaledPos.z / 100.0) - (double)zCoord >= 0.5) zCoord++;
	
	if(xCoord < 0 || xCoord > xSize - 1 || zCoord < 0 || zCoord > zSize - 1)
	{
		return NULL;
	}
	
	return this->getSquare(xCoord, zCoord);
}


Ogre::SceneNode* World::getRoot()
{
	return root;
}


ParticlePool* World::getParticlePool(std::string name)
{
	std::map<std::string, ParticlePool*>::iterator it = pools.find(name);
	return (*it).second;
}
