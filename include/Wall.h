#ifndef RAMPARTED_WALL
#define RAMPARTED_WALL

#include "Structure.h"

#include <string>
#include <vector>

class Player;
class Square;

namespace Ogre
{
	class SceneNode;
}

/// Basic wall structure.
class Wall : public Structure
{
public:
	
	/// Create a new wall in the given square.
	Wall(Square* base);
	
	/// Returns "Wall", since this is a wall.
	std::string getStructureType();

	/// Destroy the wall.
	/// Doesn't remove the wall from play, but makes the wall look broken and sets the destroyed -boolean.
	void destroy();
	
	/// Sets the correct wall mesh depending on surrounding walls.
	/// Examines the adjacent walls owned by the wall's owner, and switches the mesh to a one that makes the battlements on /// top of the wall look nicely aligned.
	void updateBattlements();

};

#endif
