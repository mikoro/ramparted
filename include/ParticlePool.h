#ifndef RAMPARTED_PARTICLEPOOL
#define RAMPARTED_PARTICLEPOOL

#include <list>
#include <string>
#include <utility>

namespace Ogre
{
	class SceneNode;
	class ParticleSystem;
}

/// This is a container for particle effects. From this class one can obtain a particle system efficiently.
class ParticlePool
{
public:

	/// Creates a particle pool containing particle systems.
	/// @param particleName Unique name for this particle pool. Preferably descriptive.
	ParticlePool(std::string particleName);

	/// This function is used to attach a particle system to a scene node.
	/// If many particle systems are needed during a short period of time the oldest one gets detached.
	/// This has no effect if that particle system is fast enough because particles can live without their emitters.
	/// @param node A particle system is attached to this scene node.
	void attachParticleTo(Ogre::SceneNode* node);

private:

	Ogre::SceneNode* root;
	std::list<std::pair<Ogre::SceneNode*, Ogre::ParticleSystem*> > particles;
	std::list<std::pair<Ogre::SceneNode*, Ogre::ParticleSystem*> >::iterator particleIterator;
};

#endif
