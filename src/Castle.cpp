#include "Castle.h"
#include "GameEngine.h"
#include "Square.h"
#include "Utility.h"
#include "Wall.h"
#include "World.h"
#include "Player.h"

#include <Ogre.h>

Castle::Castle(Square* s) {
	
	square = s;
	square->setStructure(this, 2);
	
	owner = NULL;

	Ogre::Entity* entity;
	
	root = square->getRoot()->createChildSceneNode(generateId(), Ogre::Vector3(50, 0, 50));
	root->setScale(2.0f, 2.0f, 2.0f);
	entity = GameEngine::getSingleton().getSceneManager()->createEntity(generateId(), "Castle.mesh");
	entity->setVisible(true);
	root->attachObject(entity);
	
	flag = root->createChildSceneNode(generateId(), Ogre::Vector3(0, 150, 0));
	flag->yaw(Ogre::Degree(-90));
	entity = GameEngine::getSingleton().getSceneManager()->createEntity(generateId(), "Flag.mesh");
	entity->setVisible(false);
	flag->attachObject(entity);

	flagAnim = entity->getAnimationState("fly");
	flagAnim->setEnabled(false);
	flagAnim->setLoop(true);
	
}
 
void Castle::setOwner(Player* newOwner)
{
	owner = newOwner;
	
	if(owner != NULL)
	{
		if(owner->getName() == "Player One")
		{
			((Ogre::Entity*)(flag->getAttachedObject(0)))->getSubEntity(0)->setMaterialName("FlagBlue");
		}
		if(owner->getName() == "Player Two")
		{
			((Ogre::Entity*)(flag->getAttachedObject(0)))->getSubEntity(0)->setMaterialName("FlagRed");
		}
		flag->getAttachedObject(0)->setVisible(true);
		flagAnim->setEnabled(true);
	}
	else
	{
		flag->getAttachedObject(0)->setVisible(false);
	}
}

std::string Castle::getStructureType()
{
	return "Castle";
}


