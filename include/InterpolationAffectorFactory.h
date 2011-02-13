#ifndef RAMPARTED_INTERPOLATIONAFFECTORFACTORY
#define RAMPARTED_INTERPOLATIONAFFECTORFACTORY

#include <OgreParticleAffectorFactory.h>
#include <InterpolationAffector.h>

/// 3rdparty plugin for Ogre
class InterpolationAffectorFactory: public Ogre::ParticleAffectorFactory
{
	Ogre::String getName() const{ return "Interpolation"; }

	Ogre::ParticleAffector* createAffector(Ogre::ParticleSystem* psys)
	{
		Ogre::ParticleAffector* p = new InterpolationAffector(psys);
		mAffectors.push_back(p);
		return p;
	}
};

#endif
