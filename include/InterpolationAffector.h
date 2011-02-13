#ifndef RAMPARTED_INTERPOLATIONAFFECTOR
#define RAMPARTED_INTERPOLATIONAFFECTOR

#include <OgreParticleAffector.h>

typedef std::vector<std::pair<Ogre::Real, Ogre::Real> > RealPairVector;

/// 3rdparty plugin for Ogre
class InterpolationAffector: public Ogre::ParticleAffector
{
public:

	/// 3rdparty plugin for Ogre
	class CmdRelativeVelocity: public Ogre::ParamCommand
	{
	public:
		Ogre::String doGet(const void* target) const;
		void doSet(void* target, const Ogre::String& val);
	};

	/// 3rdparty plugin for Ogre
	class CmdSpinVelocity: public Ogre::ParamCommand
	{
	public:
		Ogre::String doGet(const void* target) const;
		void doSet(void* target, const Ogre::String& val);
	};

	/// 3rdparty plugin for Ogre
	class CmdRelativeSize: public Ogre::ParamCommand
	{
	public:
		Ogre::String doGet(const void* target) const;
		void doSet(void* target, const Ogre::String& val);
	};

	/// 3rdparty plugin for Ogre
	class CmdInterpValues: public Ogre::ParamCommand
	{
	public:
		Ogre::String doGet(const void* target) const;
		void doSet(void* target, const Ogre::String& val);
	};

	/// Default constructor
	InterpolationAffector(Ogre::ParticleSystem* psys);

	/** See ParticleAffector. */
	void _affectParticles(Ogre::ParticleSystem* pSystem, Ogre::Real timeElapsed);
	/** See ParticleAffector. */
	void _initParticle(Ogre::Particle* pParticle);


	void setRelativeVelocity(bool relativeVelocity);
	void setSpinVelocity(bool spinVelocity);
	void setRelativeSize(bool relativeSize);
	void setInterpValues(std::vector<std::pair<Ogre::Real, Ogre::Real> > &interpValues);

	bool getRelativeVelocity(void) const;
	bool getSpinVelocity(void) const;
	bool getRelativeSize(void) const;
	const std::vector<std::pair<Ogre::Real, Ogre::Real> > &getInterpValues() const;


	/// Command objects
	static CmdRelativeVelocity msRelativeVelocityCmd;
	static CmdSpinVelocity msSpinVelocityCmd;
	static CmdRelativeSize msRelativeSizeCmd;
	static CmdInterpValues msInterpValuesCmd;

protected:
	Ogre::Real getValue(Ogre::Real time);

	RealPairVector mInterpValues;
	bool mRelativeVelocity;
	bool mSpinVelocity;
	bool mRelativeSize;
};

#endif
