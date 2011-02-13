#ifndef RAMPARTED_CANNON
#define RAMPARTED_CANNON

#include "Structure.h"

class BasicCannonBall;

namespace Ogre
{
	class AnimationState;
	class SceneNode;
}

/// Abstract class for describing a cannon.
class Cannon : public Structure
{
public:
		
	/// Get the current hit points of the cannon
	int getIntegrity() { return integrity; }
	
	/// Set the cannon's hit points to a given value.
	void setIntegrity(int newIntegrity)
	{
		integrity = newIntegrity;
		if(integrity <= 0) destroy();
	}
	
	/// Set the tracking of the owner's cursor on or off.
	//void setTracking(bool value);
	
	/// Fire the cannon at the owner's cursor.
	virtual void fire() = 0;
	
	/// Get the cannonball that the cannon has launched,
	/// or null if no ball is currently in the air.
	//CannonBall* getBall();

	// Returns the ball, use isReady() to check if the ball is in the air, false if it is.
	virtual BasicCannonBall* getBall() = 0;

	virtual Ogre::SceneNode* getStand() = 0;
	virtual Ogre::SceneNode* getTube() = 0;
	virtual Ogre::AnimationState* getAnim() = 0;
	/// Finds out if a the cannon is ready to fire.
	virtual bool isReady() = 0;
	virtual double getInitialVelocity() = 0;
	
protected:
	
	int integrity;
	//CannonBall* ball;
	
};

#endif
