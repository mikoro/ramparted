#include "Wall.h"
#include "GameEngine.h"
#include "ParticlePool.h"
#include "Square.h"
#include "Utility.h"
#include "World.h"
#include "SoundManager.h"

#include <Ogre.h>

Wall::Wall(Square* base)
{
	square = base;
	square->setStructure(this, 1);
	square->setOwner(NULL);
	
	destroyed = false;
	
	owner = NULL;
	
	Ogre::Entity* entity;
	
	GameEngine& engine = GameEngine::getSingleton();

	root = square->getRoot()->createChildSceneNode(generateId());
	entity = engine.getSceneManager()->createEntity(generateId(), "Wall-0.mesh");
	entity->setVisible(true);
	root->attachObject(entity);
}

void Wall::destroy()
{
	if(this->destroyed) return;
	
	destroyed = true;
	
	root->detachAllObjects();
	
	Ogre::Entity* ent = GameEngine::getSingleton().getSceneManager()->createEntity(generateId(), "WallBurnt.mesh");
	root->attachObject(ent);
	root->yaw(Ogre::Degree(90 * (rand() % 4)));

	GameEngine::getSingleton().getSoundManager()->play("Explosion");

	square->getWorld()->getParticlePool("ExplosionSmoke")->attachParticleTo(root);
	square->getWorld()->getParticlePool("ExplosionSolid")->attachParticleTo(root);
}


void Wall::updateBattlements()
{	
	if(this->destroyed) return;
	
	unsigned char adjacents = 0x00;
	unsigned char mask = 0x80;
	
	// Find directions in which there are adjacent walls
	for(int d = 0; d < 8; d++)
	{
		Square* sqr = square->getAdjacent(d);
		
		if(sqr != NULL &&
		   sqr->getStructure() != NULL &&
		   !sqr->getStructure()->isDestroyed() &&
		   sqr->getStructure()->getOwner() == owner &&
		   sqr->getStructure()->getStructureType() == "Wall")
		{
			adjacents |= mask;
		}
		
		mask >>= 1;
	}
	
	// Remove corner adjacents that are not between two side adjacents,
	// thereby reducing possible combinations
	if(!((adjacents & 0x80) && (adjacents & 0x20))) adjacents &= 0xBF;
	if(!((adjacents & 0x20) && (adjacents & 0x08))) adjacents &= 0xEF;
	if(!((adjacents & 0x08) && (adjacents & 0x02))) adjacents &= 0xFB;
	if(!((adjacents & 0x02) && (adjacents & 0x80))) adjacents &= 0xFE;
	
	unsigned char mesh = 0x00;
	int rotation;
	
	// Find a wall mesh corresponding to the directions of adjacents by
	// comparing the directions to mesh signatures
	for(rotation = 0; rotation < 4; rotation++)
	{
		if(shiftRight(2*rotation, adjacents) == 0x02) { mesh = 0x02; break; }
		if(shiftRight(2*rotation, adjacents) == 0x82) { mesh = 0x82; break; }
		if(shiftRight(2*rotation, adjacents) == 0x83) { mesh = 0x83; break; }
		if(shiftRight(2*rotation, adjacents) == 0x22) { mesh = 0x22; break; }
		if(shiftRight(2*rotation, adjacents) == 0xE2) { mesh = 0xE2; break; }
		if(shiftRight(2*rotation, adjacents) == 0xA2) { mesh = 0xA2; break; }
		if(shiftRight(2*rotation, adjacents) == 0xA3) { mesh = 0xA3; break; }
		if(shiftRight(2*rotation, adjacents) == 0xE3) { mesh = 0xE3; break; }
		if(shiftRight(2*rotation, adjacents) == 0xBB) { mesh = 0xBB; break; }
		if(shiftRight(2*rotation, adjacents) == 0xAB) { mesh = 0xAB; break; }
		if(shiftRight(2*rotation, adjacents) == 0xEB) { mesh = 0xEB; break; }
		if(shiftRight(2*rotation, adjacents) == 0xEF) { mesh = 0xEF; break; }
		if(shiftRight(2*rotation, adjacents) == 0xAA) { mesh = 0xAA; break; }
		if(shiftRight(2*rotation, adjacents) == 0xFF) { mesh = 0xFF; break; }
	}
	
	// Convert the mesh signature into a filename and create the wall entity
	std::stringstream aux;
	aux << "Wall-" << std::hex << std::uppercase << (int)mesh << std::uppercase << ".mesh";
	GameEngine& engine = GameEngine::getSingleton();
	Ogre::Entity* entity = engine.getSceneManager()->createEntity(generateId(), aux.str());
	entity->setVisible(true);
	
	// Remove previous mesh
	root->detachAllObjects();
	root->resetOrientation();
	
	// Add new mesh and rotate into proper position
	root->attachObject(entity);
	root->yaw(Ogre::Degree(90 * rotation));
	
}


std::string Wall::getStructureType()
{
	return "Wall";
}
