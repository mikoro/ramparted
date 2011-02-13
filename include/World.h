#ifndef RAMPARTED_WORLD
#define RAMPARTED_WORLD

#include <map>
#include <vector>

class ParticlePool;
class Player;
class Square;

namespace Ogre
{
	class SceneNode;
	class Vector3;
}

/// Contains all the squares, structures etc.
class World
{
public:

	/// Create and initialize a new world with given dimensions in squares.
	/// @param origin The parent node of the world's root node.
	/// @param x The width of the world along the x-axis.
	/// @param z The width of the world along the z-axis.
	World(Ogre::SceneNode* origin, int x, int z);
	
	/// Get the world's root node.
	Ogre::SceneNode* getRoot();

	/// Get a square in the world at the given coordinates.
	/// Returns NULL if the coordinates are out of bounds.
	/// @param x The square's x-coordinate.
	/// @param z The square's z-coordinate.
	Square* getSquare(int x, int z);

	/// Get the square that a vector from the world's root node points at.
	/// Returns NULL if the vector points outside the square matrix.
	Square* getSquare(Ogre::Vector3 pos);

	/// Get the square that a vector from the Ogre's world root node points at.
	/// Returns NULL if the vector points outside the square matrix.
	Square* getSquareO(Ogre::Vector3 pos);

	/// Get a particle pool with its name.
	ParticlePool* getParticlePool(std::string);

	
private:
	
	int xSize;
	int zSize;
	Ogre::SceneNode* root;
	std::vector<std::vector<Square*> > squares;
	std::map<std::string, ParticlePool*> pools;
	
};

#endif
