#ifndef RAMPARTED_CURSOR
#define RAMPARTED_CURSOR

#include <OGRE/OgreTimer.h>
#include <OGRE/OgreVector3.h>
#include <OGRE/OgreQuaternion.h>
#include <vector>

class Player;
class Wall;
class World;
class Cannon;

namespace Ogre
{
	class SceneNode;
}

/// Manage the player's cursor.
class Cursor
{
public:

	/// Create a new instance.
	/// @param owner Player who owns this cursor.
	/// @param world World this cursor belongs to.
	Cursor(Player* owner, World* world);

	/// All the possible actions a cursor can do.
	enum CursorActions { MoveUp = 1, MoveDown = 2, MoveLeft = 4, MoveRight = 8, Rotate = 16 };

	/// All the modes a cursor can be in.
	enum CursorModes { ModeWalls, ModeAim , ModeCannon};

	/// Set an action to be committed.
	/// Can be called multiple times with multiple actions, all will be executed by calling update.
	/// @param action A CursorActions enumeration value.
	void setAction(CursorActions action);

	/// Change the cursor's mode.
	/// @param mode A CursorModes enumeration value.
	void setMode(CursorModes mode);

	/// Put current walls into parent world.
	/// This will work only in ModeWalls and will automatically generate new random wall.
	bool placeWalls();

	/// Put a cannon into to the world.
	/// Can only be called while in the ModeCannon mode.
	bool placeCannon();

	/// After placing anything, new placing is not possible before calling this.
	/// This can be used to enforce user to relese a button to commit the same action again.
	void allowPlacing();

	/// Update the cursor.
	/// Depending on the mode, the cursor will be moved to its new position.
	/// @param timeStep Time since last call.
	void update(double timeStep);

	/// Render the cursor.
	/// @param interpolation Interpolation value between update calls.
	void render(double interpolation);

	/// Get the cursor's world position.
	Ogre::Vector3 getPosition();

	/// Get the cursor's CursorMode.
	int getMode();

private:

	/// Generate new walls for placement mode.
	void generateWalls();

	/// Remove all walls or the cannon from the cursor.
	void clearGrid();

	struct States
	{
		Ogre::Vector3 position;
		Ogre::Quaternion orientation;
		bool moving;
		bool rotating;
	};

	World* world;
	World* grid;
	Player* owner;
	Ogre::SceneNode* root;
	std::vector<Wall*> walls;
	Cannon* cannon;
	int direction;
	Ogre::Timer timer;
	States previousStates, currentStates;
	CursorModes mode;
	int actions;
	double lastMoveTime;
	double lastRotateTime;
	bool startedMoving;
	bool startedRotating;
	bool canPlace;
};

#endif
