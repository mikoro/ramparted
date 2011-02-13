#ifndef RAMPARTED_SQUARE
#define RAMPARTED_SQUARE

class Player;
class Structure;
class World;

namespace Ogre
{
	class SceneNode;
}

/// Describes one square in the World.
class Square
{
public:

	/// Create a new square with the given coordinates.
	/// This should only be called by the world constructor.
	/// @param w The world the square will be added to.
	/// @param x X position
	/// @param z Z position
	Square(World* w, int x, int z);

	/// Get the square's root node.
	Ogre::SceneNode* getRoot();

	/// Get the world the square is part of.
	World* getWorld();

	/// Get the owner of the square.
	Player* getOwner();

	/// Set the owner of the square.
	void setOwner(Player* newOwner);

	/// Get the structure in the square.
	/// Returns null if the square is empty.
	Structure* getStructure();

	/// Set the structure that is in this square.
	/// @param newStructure The structure to be set as the structure in the square.
	/// @param size The width of the structure in squares. If larger than 1, the appropriate additional squares will be set to contain the structure.
	void setStructure(Structure* newStructure, int size);

	/// Get the adjacent square in the given direction.
	/// 0 is north (negative z-direction), 1 northeast, 2 east (positive x-direction) etc.
	/// Returns NULL if there is no square in that direction.
	Square* getAdjacent(int direction);

	/// Mark the square. Used by the flood fill algorithm in Player::floodMark() to determine in which squares it has visited.
	void mark(bool value) { marked = value; }
	
	/// Determine if the square is marked.
	bool isMarked() { return marked; }
	
	/// Set the square as a boundary square.
	void setBoundary(bool value) { boundary = value; }
	
	/// Determine if the square is a boundary square.
	bool isBoundary() { return boundary; }

private:

	Ogre::SceneNode* root;
	World* world;
	Player* owner;
	Structure* structure;
	int xPos;
	int zPos;
	bool marked;
	bool boundary;
};

#endif
