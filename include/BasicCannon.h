#ifndef RAMPARTED_BASICCANNON
#define RAMPARTED_BASICCANNON

#include <Ogre.h>
#include "Cannon.h"

class BasicCannonBall;

namespace Ogre
{
	class AnimationState;
	class ParticleSystem;
	class SceneNode;
}

/// Implements most basic cannon type.
class BasicCannon : public Cannon
{
public:
	
	/// Create a new basic cannon.
	/// @param s The square the cannon will be added to.
	BasicCannon(Square* s);
	
	void setOwner(Player* newOwner);
	
	/// Returns "BasicCannon", since this is a Cannon.
	std::string getStructureType() { return "BasicCannon"; }

	/// Fire the cannon at the owner's cursor.
	void fire();
	
	/// Destroy the cannon.
	void destroy();

	Ogre::SceneNode* getStand();
	Ogre::SceneNode* getTube();
	Ogre::AnimationState* getAnim();
	BasicCannonBall* getBall();
	bool isReady();
	/// Get cannons muzzle velocity.
	double getInitialVelocity();

	/// Update cannons orientation.
	void update(Ogre::Quaternion standOrientation, Ogre::Quaternion tubeOrientation);
	/// Render cannons orientation.
	void render(double interpolation);
	
private:

	struct States
	{
		Ogre::Quaternion standOrientation;
		Ogre::Quaternion tubeOrientation;
	};
	
	Ogre::SceneNode* stand;
	Ogre::SceneNode* tube;
	BasicCannonBall* ball;
	double initialVelocity;
	States previousStates, currentStates;

	Ogre::ParticleSystem* muzzleFlashParticleSystem;
	Ogre::ParticleSystem* muzzleSmokeParticleSystem;
	Ogre::AnimationState* cannonAnim;

	
		
};

#endif

