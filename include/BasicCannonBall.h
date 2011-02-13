#ifndef RAMPARTED_BASICCANNONBALL
#define RAMPARTED_BASICCANNONBALL

#include <OGRE/OgreVector3.h>

class Cannon;

namespace Ogre
{
	class SceneNode;
}

/// Implements most basic cannon ball type.
class BasicCannonBall
{
public:
	/// Creates a cannon ball for a particular cannon. One cannon can only have one cannon ball.
	/// The newly created cannon ball is located in its owner cannons tube.
	/// @param cannon owner cannon.
	BasicCannonBall(Cannon* cannon);

	Ogre::SceneNode* getRoot() {return root; }

	/// Send the cannon ball to the direction where owner cannons tube points at.
	void launch();

	/// This is to be called when a cannon ball hits the ground or water.
	/// Adds right kind of particle effects in the hit position, plays the right sound, and modifies the ground if hit.
	/// After this function is called the cannon ball is located in its original position.
	void hit();

	/// @return True if this cannon ball can be launced, that is if this ball is not in the air.
	bool isReady();

	/// @return Current velocity. Velocity contains information of the balls current heading and speed.
	Ogre::Vector3 getVelocity();

	/// Change balls current velocity. This should be changed every in every logic tick so that the ball follows a natural trajectory of a projectile.
	/// @param newVelocity Balls new velocity.
	void setVelocity(Ogre::Vector3 newVelocity);

private:
	Ogre::SceneNode* root;
	Ogre::SceneNode* explosionHolder;
	Ogre::Vector3 velocity;
	Cannon* cannon;
	bool ready;
};

#endif
