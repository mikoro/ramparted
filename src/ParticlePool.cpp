#include "ParticlePool.h"
#include "GameEngine.h"
#include "Utility.h"

#include <Ogre.h>

ParticlePool::ParticlePool(std::string particleName)
{
	root = GameEngine::getSingleton().getSceneManager()->createSceneNode(generateId());
	for(int i = 0; i < 3; ++i)
	{
		Ogre::ParticleSystem* sys = GameEngine::getSingleton().getSceneManager()->createParticleSystem(generateId(), particleName);
		root->attachObject(sys);
		particles.push_back(std::make_pair(root, sys));
	}

	particleIterator = particles.begin();
}

void ParticlePool::attachParticleTo(Ogre::SceneNode* node)
{
	try
	{
		(*particleIterator).first->detachObject((*particleIterator).second->getName());
	}
	catch(Ogre::ItemIdentityException e)
	{
		GameEngine::getSingletonPtr()->getLog()->logMessage("Caught an exception: " + e.getFullDescription());
	}

	(*particleIterator).first = node;
	(*particleIterator).first->attachObject((*particleIterator).second);
	
	(*particleIterator).second->getEmitter(0)->setEnabled(true);

	particleIterator++;
	if(particleIterator == particles.end())
	{
		particleIterator = particles.begin();
	}
}
