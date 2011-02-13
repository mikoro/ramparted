#ifndef RAMPARTED_CASTLE
#define RAMPARTED_CASTLE

#include "Structure.h"

#include <string>
#include <Ogre.h>

class Player;
class Square;

/// Basic castle.
class Castle : public Structure
{
public:
	
	/// Create a new castle in the given square.
	Castle(Square* s);
	
	/// Set the owner of the castle.
	/// If the new owner is a player, a flag is raised on top of the castle.
	/// If the new owner is NULL, any existing flag is hidden.
	void setOwner(Player* newOwner);
	
	/// Returns "Castle", since this is a castle.
	std::string getStructureType();

	Ogre::AnimationState* getFlagAnim() { return flagAnim; }
	
private:
	
	Ogre::SceneNode* flag;
	Ogre::AnimationState* flagAnim;
	
};

#endif
