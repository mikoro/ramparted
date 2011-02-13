#ifndef RAMPARTED_STRUCTURE
#define RAMPARTED_STRUCTURE

#include <string>

class Player;
class Square;

namespace Ogre
{
	class SceneNode;
}

/// Describes one structure on the Square.
class Structure
{
public:
	
	virtual ~Structure() {};
	
	/// Get the structure's root node.
	Ogre::SceneNode* getRoot() { return root; }
	
	/// Get the square the structure is in.
	/// If the structure is larger than one square, the square that the structure was placed in is returned (that is, the /// square closest to the world root.
	Square* getSquare() { return square; }
	
	/// Get the owner of the structure.
	Player* getOwner() { return owner; }
	
	/// Set the owner of the structure.
	virtual void setOwner(Player* newOwner) { owner = newOwner; }
	
	/// Get the type of the structure as a string.
	virtual std::string getStructureType() = 0;
	
	/// Destroy the structure.
	virtual void destroy() { destroyed = true; }
	
	/// Determine if the structure is destroyed.
	bool isDestroyed() { return destroyed; }
	
protected:
	
	Ogre::SceneNode* root;
	Square* square;
	Player* owner;
	bool destroyed;

};
	
#endif
