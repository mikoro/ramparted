#include "BasicCannon.h"
#include "BasicCannonBall.h"
#include "GameEngine.h"
#include "ParticlePool.h"
#include "SoundManager.h"
#include "Square.h"
#include "Utility.h"
#include "World.h"
#include "Player.h"

#include <Ogre.h>

BasicCannon::BasicCannon(Square* s)
{
	square = s;
	square->setStructure(this, 2);
	
	integrity = 10;
	initialVelocity = 2100.0;
	destroyed = false;
	
	Ogre::Entity* entity;
	
	GameEngine& engine = GameEngine::getSingleton();

	root = square->getRoot()->createChildSceneNode(generateId(), Ogre::Vector3(50, 0, 50));
	root->setScale(0.7f, 0.7f, 0.7f);
	entity = engine.getSceneManager()->createEntity(generateId(), "CannonBase.mesh");
	entity->setVisible(true);
	root->attachObject(entity);
	
	stand = root->createChildSceneNode(generateId());
	entity = engine.getSceneManager()->createEntity(generateId(), "CannonStand.mesh");
	entity->setVisible(true);
	stand->attachObject(entity);
	
	tube = stand->createChildSceneNode(generateId(), Ogre::Vector3(0, 185, 25));
	entity = engine.getSceneManager()->createEntity(generateId(), "CannonTube.mesh");
	entity->setVisible(true);
	tube->attachObject(entity);
	tube->pitch(Ogre::Degree(-45));

	cannonAnim = entity->getAnimationState("shoot");
	cannonAnim->setEnabled(false);
	cannonAnim->setLoop(false);

	// TODO: Maybe the particle systems should be scaled bigger some how
	muzzleFlashParticleSystem = engine.getSceneManager()->createParticleSystem(generateId(), "MuzzleFlashParticle");
	Ogre::SceneNode* muzzleFlashNode = tube->createChildSceneNode(generateId(), Ogre::Vector3(0, 0, 180));
	muzzleFlashNode->attachObject(muzzleFlashParticleSystem);
	muzzleFlashParticleSystem->getEmitter(0)->setEnabled(false);
	muzzleSmokeParticleSystem = engine.getSceneManager()->createParticleSystem(generateId(), "MuzzleSmokeParticle");
	Ogre::SceneNode* muzzleSmokeNode = tube->createChildSceneNode(generateId(), Ogre::Vector3(0, 0, 100));
	muzzleSmokeNode->attachObject(muzzleSmokeParticleSystem);
	muzzleSmokeParticleSystem->getEmitter(0)->setEnabled(false);

	ball = new BasicCannonBall(this);
}

void BasicCannon::setOwner(Player* newOwner)
{
	if(owner != newOwner)
	{		
		owner = newOwner;
		
		if(owner != NULL)
		{
			owner->addCannon(this);
		}
	}
}                               

void BasicCannon::destroy()
{
	if(destroyed) return;

	integrity--;

	if(integrity <= 0)
	{
		destroyed = true;

		square->getWorld()->getParticlePool("ExplosionSmoke")->attachParticleTo(root);
		square->getWorld()->getParticlePool("ExplosionSolid")->attachParticleTo(root);
		GameEngine::getSingletonPtr()->getSoundManager()->play("Explosion");

		Ogre::Entity* ent = GameEngine::getSingletonPtr()->getSceneManager()->createEntity(generateId(), "CannonBurnt.mesh");
		stand->removeAllChildren();
		stand->detachAllObjects();
		stand->attachObject(ent);
	}
}

Ogre::SceneNode* BasicCannon::getStand()
{
	return stand;
}

Ogre::SceneNode* BasicCannon::getTube()
{
	return tube;
}

Ogre::AnimationState* BasicCannon::getAnim()
{
	return cannonAnim;
}

BasicCannonBall* BasicCannon::getBall()
{
	return ball;
}


bool BasicCannon::isReady()
{
	if(!cannonAnim->getEnabled() && ball->isReady())
		return true;
	else
		return false;
}

double BasicCannon::getInitialVelocity()
{
	return initialVelocity;
}


void BasicCannon::fire()
{
	if(!cannonAnim->getEnabled() && !destroyed)
	{
		cannonAnim->setEnabled(true);
		GameEngine::getSingleton().getSoundManager()->play("Cannon", false);
		muzzleFlashParticleSystem->getEmitter(0)->setEnabled(true);
		muzzleSmokeParticleSystem->getEmitter(0)->setEnabled(true);

		ball->launch();
	}
}

void BasicCannon::update(Ogre::Quaternion standOrientation, Ogre::Quaternion tubeOrientation)
{
	if(!destroyed)
	{
		previousStates.standOrientation = currentStates.standOrientation;
		previousStates.tubeOrientation = currentStates.tubeOrientation;

		currentStates.standOrientation = standOrientation;
		currentStates.tubeOrientation = tubeOrientation;
	}
}

void BasicCannon::render(double interpolation)
{
	if(!destroyed)
	{
		tube->setOrientation(previousStates.tubeOrientation * (1.0 - interpolation) + currentStates.tubeOrientation * interpolation);
		stand->setOrientation(previousStates.standOrientation * (1.0 - interpolation) + currentStates.standOrientation * interpolation);
	}
}
