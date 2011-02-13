#ifndef RAMPARTED_PLAYER
#define RAMPARTED_PLAYER

#include <string>
#include <vector>

class Cannon;
class Castle;
class Cursor;
class Square;
class Wall;
class World;

/// Manages all player's actions.
class Player
{
public:
	
	/// Create a new player.
	/// @param world The world to associate this player with.
	/// @param name The player's name.
	Player(World* world, std::string name);
	
	/// Add a wall to the player's collection and set the player as the wall's owner. 
	void addWall(Wall* wall);
	
	/// Add a cannon to the player's collection if not already there and set the player as the cannons's owner.
	void addCannon(Cannon*);
	
	/// Fires one intact & ready (no cannonball in the air) cannon, owned by the player, at the player's cursor.
	void requestFire();
	
	/// Allow this player to fire cannons.
	void allowFiring();
	
	/// Update the meshes of all the player's intact walls so that their battlements align nicely.
	void updateWalls();

	/// Orders all players cannons to update their orientation.
	/// This function should be called from the logic tick.
	/// @param timeStep time step from logic tick.
	void updateCannons(double timeStep);

	/// Orders all players cannons to be rendered interpolated. This also enables cannons mesh animations.
	/// @param interpolation interpolation value used for smooth animation.
	/// @param timeStep time step for mesh animations.
	void renderCannons(double interpolation, double timeStep);
	
	/// Remove any destroyed walls owned by the player.
	/// The walls are permanently removed from the game and the player's possession.
	void removeDeadWalls();
	
	/// Set the owner of all squares owned by the player to NULL.
	void clearSquares();
	
	/// Remove all cannons from the player's possession and set their owner as NULL.
	void clearCannons();
	
	/// Find all areas enclosed by the player's walls and set the squares in those areas as owned by the player.
	/// Uses floodMark starting at the square at coordinates (0, 0) to find squares outside the walls, so that the inverse of those is inside the walls.
	/// All enclosed castles and cannons are also set as owned by the player.
	/// Walls are always located in neutral squares, they are not included in the enclosed area.
	int claimSquares();

	/// Get the player's name.
	std::string getName();
	
	/// Get the player's score.
	int getScore();
	
	/// Get the player's cursor.
	Cursor* getCursor();
	
	/// Get all cannons owned by the player.
	std::vector<Cannon*> getCannons();
		
private:
	
	/// Used by claimSquares to find areas enclosed by the player's walls. Starts at the given square and marks all squares it can reach with a flood fill algorithm.
	void floodMark(Square* sqr);
	
	World* world;
	std::string name;
	Cursor* cursor;
	int score;
	bool canFire;

	std::vector<Wall*> walls;
	std::vector<Cannon*> cannons;

};

#endif
