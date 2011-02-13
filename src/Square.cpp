#include "Square.h"
#include "GameEngine.h"
#include "Player.h"
#include "Utility.h"
#include "World.h"
#include "Structure.h"

#include <Ogre.h>

Square::Square(World* w, int x, int z)
{
	world = w;
	root = world->getRoot()->createChildSceneNode(generateId(), Ogre::Vector3(x*100, 0, z*100 ));
	Ogre::Entity* floor = GameEngine::getSingleton().getSceneManager()->createEntity(generateId(), "Floor.mesh");
	root->attachObject(floor);
	root->setVisible(false);
	
	owner = NULL;
	structure = NULL;
	xPos = x;
	zPos = z;
	marked = false;
	boundary = false;
}


void Square::setStructure(Structure* newStructure, int size)
{
	if(size == 1)
	{
		structure = newStructure;
	}
	else if(size == 2)
	{
		structure = newStructure;
		
		this->getAdjacent(6)->setStructure(newStructure, 1);
		this->getAdjacent(7)->setStructure(newStructure, 1);
		this->getAdjacent(0)->setStructure(newStructure, 1);
	}
	else if(size == 3)
	{
		structure = newStructure;
		
		this->getAdjacent(6)->setStructure(newStructure, 2);
		this->getAdjacent(7)->setStructure(newStructure, 2);
		this->getAdjacent(0)->setStructure(newStructure, 2);
	}
}


Square* Square::getAdjacent(int direction)
{
	if(direction == 0) return world->getSquare(xPos,     zPos + 1);
	if(direction == 1) return world->getSquare(xPos - 1, zPos + 1);
	if(direction == 2) return world->getSquare(xPos - 1, zPos    );
	if(direction == 3) return world->getSquare(xPos - 1, zPos - 1);
	if(direction == 4) return world->getSquare(xPos,     zPos - 1);
	if(direction == 5) return world->getSquare(xPos + 1, zPos - 1);
	if(direction == 6) return world->getSquare(xPos + 1, zPos    );
	if(direction == 7) return world->getSquare(xPos + 1, zPos + 1);
	return NULL;
}


void Square::setOwner(Player* newOwner)
{
	owner = newOwner;
	if(owner != NULL)
	{
		if(owner->getName() == "Player One")
		{
			((Ogre::Entity*)(root->getAttachedObject(0)))->getSubEntity(0)->setMaterialName("FloorSub_blue");
		}
		if(owner->getName() == "Player Two")
		{
			((Ogre::Entity*)(root->getAttachedObject(0)))->getSubEntity(0)->setMaterialName("FloorSub_red");
		}
		root->getAttachedObject(0)->setVisible(true);
	}
	else
	{
		root->getAttachedObject(0)->setVisible(false);
	}
	
	if(structure != NULL && structure->getOwner() != newOwner)
	{
		structure->setOwner(newOwner);
	}
}


Ogre::SceneNode* Square::getRoot()
{
	return root;
}


World* Square::getWorld()
{
	return world;
}


Player* Square::getOwner()
{
	return owner;
}


Structure* Square::getStructure()
{
	return structure;
}


