#include "Cursor.h"
#include "GameEngine.h"
#include "Player.h"
#include "Square.h"
#include "Utility.h"
#include "Wall.h"
#include "World.h"
#include "BasicCannon.h"

#include <Ogre.h>

const double longDelay = 0.2;
const double mediumDelay = 0.1;
const double shortDelay = 0.05;

Cursor::Cursor(Player* owner, World* world)
{
	this->owner = owner;
	this->world = world;
	
	root = world->getRoot()->createChildSceneNode(generateId());
	root->setScale(2.0f, 2.0f, 2.0f);
	
	Ogre::Entity* entity = GameEngine::getSingleton().getSceneManager()->createEntity(generateId(), "Cursor.mesh");
	root->attachObject(entity);
	
	grid = new World(root, 5, 5);
	grid->getRoot()->setScale(0.5f, 0.5f, 0.5f);
	grid->getRoot()->translate(Ogre::Vector3(-100, -50, -100));
	
	direction = 0;
	
	cannon = NULL;

	previousStates.position = currentStates.position = Ogre::Vector3(150, 100, 150);
	actions = 0;
	lastMoveTime = lastRotateTime = 0;
	startedMoving = startedRotating = false;
	canPlace = true;
	setMode(ModeAim);
}

void Cursor::setAction(CursorActions action)
{
	actions |= action;
}

void Cursor::setMode(CursorModes mode)
{
	this->mode = mode;

	if(mode == ModeAim)
	{
		root->resetOrientation();
		direction = 0;
		
		clearGrid();
	}
	
	else if(mode == ModeWalls)
	{
		previousStates.position = currentStates.position = world->getSquare(root->getPosition())->getRoot()->getPosition() + Ogre::Vector3(0, 100, 0);
		root->resetOrientation();
		direction = 0;

		owner->removeDeadWalls();
		owner->updateWalls();
		owner->clearSquares();
		owner->clearCannons();
		owner->claimSquares();
		
		generateWalls();
	}

	else if(mode == ModeCannon)
	{
		previousStates.position = currentStates.position = world->getSquare(root->getPosition())->getRoot()->getPosition() + Ogre::Vector3(0, 100, 0);
		root->resetOrientation();
		direction = 0;
		
		clearGrid();
		
		cannon = new BasicCannon(grid->getSquare(1, 1));
	}
}
	
bool Cursor::placeCannon()
{
	if(mode != ModeCannon || !canPlace)
		return false;		
	
	Ogre::Vector3 cursorToSquare = cannon->getSquare()->getRoot()->getPosition() + Ogre::Vector3(-200, 0, -200);
	cursorToSquare = rotateClockwise(cursorToSquare, direction / 2);
	Square* worldSquare = world->getSquare(root->getPosition() + cursorToSquare);

	if(worldSquare == NULL ||
	   worldSquare->isBoundary() ||
	   worldSquare->getOwner() != owner || 
	   worldSquare->getStructure() != NULL) return false;
	
	if(worldSquare->getAdjacent(6) == NULL || 
	   worldSquare->getAdjacent(6)->isBoundary() || 
	   worldSquare->getAdjacent(6)->getOwner() != owner || 
	   worldSquare->getAdjacent(6)->getStructure() != NULL) return false;
	
	if(worldSquare->getAdjacent(7) == NULL || 
	   worldSquare->getAdjacent(7)->isBoundary() || 
	   worldSquare->getAdjacent(7)->getOwner() != owner || 
	   worldSquare->getAdjacent(7)->getStructure() != NULL) return false;
	
	if(worldSquare->getAdjacent(0) == NULL || 
	   worldSquare->getAdjacent(0)->isBoundary() || 
	   worldSquare->getAdjacent(0)->getOwner() != owner || 
	   worldSquare->getAdjacent(0)->getStructure() != NULL) return false;
	
	owner->addCannon(new BasicCannon(worldSquare));
	
	canPlace = false;
	return true;	
}

bool Cursor::placeWalls()
{
	if(mode != ModeWalls || !canPlace)
		return false;

	for(unsigned int i = 0; i < walls.size(); i++)
	{
		Ogre::Vector3 cursorToSquare = walls[i]->getSquare()->getRoot()->getPosition() + Ogre::Vector3(-200, 0, -200);
		cursorToSquare = rotateClockwise(cursorToSquare, direction / 2);
		Square* worldSquare = world->getSquare(root->getPosition() + cursorToSquare);
		
		if(worldSquare == NULL || worldSquare->isBoundary() || worldSquare->getStructure() != NULL)
			return false;
	}
	
	for(unsigned int i = 0; i < walls.size(); i++)
	{
		Ogre::Vector3 cursorToSquare = walls[i]->getSquare()->getRoot()->getPosition() + Ogre::Vector3(-200, 0, -200);
		cursorToSquare = rotateClockwise(cursorToSquare, direction / 2);
		Square* worldSquare = world->getSquare(root->getPosition() + cursorToSquare);
		
		owner->addWall(new Wall(worldSquare));
	}

	canPlace = false;
	generateWalls();
	return true;
}

void Cursor::allowPlacing()
{
	canPlace = true;
}

void Cursor::update(double timeStep)
{
	if(mode == ModeWalls || mode == ModeCannon)
	{
		if(currentStates.moving == true && previousStates.moving == false)
			startedMoving = true;

		if(currentStates.rotating == true && previousStates.rotating == false)
			startedRotating = true;

		double currentTime = (double)timer.getMicroseconds() / 1000000.0;
		bool canMove = !currentStates.moving || (currentStates.moving && (currentTime - lastMoveTime) > (startedMoving ? longDelay : shortDelay));
		bool canRotate = !currentStates.rotating || (currentStates.rotating && (currentTime - lastRotateTime) > (startedRotating ? longDelay : mediumDelay));

		if(canMove)
		{
			lastMoveTime = currentTime;
			startedMoving = false;
		}

		if(canRotate)
		{
			lastRotateTime = currentTime;
			startedRotating = false;
		}

		Ogre::Vector3 translation(0, 0, 0);

		if(canMove && (actions & MoveUp) && 
		   world->getSquare(root->getPosition())->getAdjacent(0) != NULL &&
		   !world->getSquare(root->getPosition())->getAdjacent(0)->isBoundary())
		{
			translation.z += 100;
		}

		if(canMove && (actions & MoveRight) && 
		   world->getSquare(root->getPosition())->getAdjacent(2) != NULL &&
		   !world->getSquare(root->getPosition())->getAdjacent(2)->isBoundary())
		{
			translation.x -= 100;
		}

		if(canMove && (actions & MoveDown) && 
		   world->getSquare(root->getPosition())->getAdjacent(4) != NULL &&
		   !world->getSquare(root->getPosition())->getAdjacent(4)->isBoundary())
		{
			translation.z -= 100;
		}

		if(canMove && (actions & MoveLeft) && 
		   world->getSquare(root->getPosition())->getAdjacent(6) != NULL &&
		   !world->getSquare(root->getPosition())->getAdjacent(6)->isBoundary())
		{
			translation.x += 100;
		}

		previousStates = currentStates;
		currentStates.position += translation;

		if(mode == ModeWalls && canRotate && (actions & Rotate))
		{
			//currentStates.orientation = Ogre::Quaternion(Ogre::Degree(90), Ogre::Vector3::UNIT_Y) * currentStates.orientation;

			root->yaw(Ogre::Degree(-90));
			direction = ((direction + 2) % 8);
		}

		currentStates.moving = (actions & MoveUp) || (actions & MoveDown) || (actions & MoveLeft) || (actions & MoveRight);
		currentStates.rotating = (actions & Rotate) != 0;
	}
	else if(mode == ModeAim)
	{
		Ogre::Vector3 translation(0, 0, 0);

		if(actions & MoveUp)
			translation.z += 750;

		if(actions & MoveDown)
			translation.z -= 750;

		if(actions & MoveLeft)
			translation.x += 750;

		if(actions & MoveRight)
			translation.x -= 750;

		previousStates = currentStates;
		currentStates.position += (translation * timeStep);
	}

	actions = 0;
}

void Cursor::render(double interpolation)
{
	if(mode == ModeWalls || mode == ModeCannon)
		root->setPosition(currentStates.position);
	else if(mode == ModeAim)
		root->setPosition(previousStates.position * (1.0 - interpolation) + currentStates.position * interpolation);

	//root->setOrientation(currentStates.orientation);
}

Ogre::Vector3 Cursor::getPosition()
{
	return root->getWorldPosition();
}

int Cursor::getMode()
{
	return mode;
}

void Cursor::generateWalls()
{
	if(mode != ModeWalls)
		return;

	clearGrid();

	switch(rand() % 16)
	{
		// one square block
		case 0:
		{
			walls.push_back(new Wall(grid->getSquare(2, 2)));
		} break;
		
		// U-block
		case 1:
		{
			walls.push_back(new Wall(grid->getSquare(1, 1)));
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(1, 3)));
			walls.push_back(new Wall(grid->getSquare(2, 3)));
			walls.push_back(new Wall(grid->getSquare(3, 3)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 1)));
		} break;

		// medium straight block
		case 2:
		{
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 3)));
		} break;

		// long S-block
		case 3:
		{
			walls.push_back(new Wall(grid->getSquare(1, 0)));
			walls.push_back(new Wall(grid->getSquare(1, 1)));
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 3)));
			walls.push_back(new Wall(grid->getSquare(3, 4)));
		} break;

		// long straight block
		case 4:
		{
			walls.push_back(new Wall(grid->getSquare(2, 0)));
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 3)));
			walls.push_back(new Wall(grid->getSquare(2, 4)));
		} break;

		// small corner
		case 5:
		{
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 1)));
		} break;

		// small L-block
		case 6:
		{
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 1)));
		} break;

		// big L-block
		case 7:
		{
			walls.push_back(new Wall(grid->getSquare(1, 0)));
			walls.push_back(new Wall(grid->getSquare(2, 0)));
			walls.push_back(new Wall(grid->getSquare(3, 0)));
			walls.push_back(new Wall(grid->getSquare(3, 1)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 3)));
			walls.push_back(new Wall(grid->getSquare(3, 4)));
		} break;

		// long Z-block
		case 8:
		{
			walls.push_back(new Wall(grid->getSquare(1, 0)));
			walls.push_back(new Wall(grid->getSquare(2, 0)));
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 3)));
			walls.push_back(new Wall(grid->getSquare(2, 4)));
			walls.push_back(new Wall(grid->getSquare(3, 4)));
		} break;

		// short Z-block
		case 9:
		{
			walls.push_back(new Wall(grid->getSquare(1, 1)));
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 3)));
		} break;

		// big corner
		case 10:
		{
			walls.push_back(new Wall(grid->getSquare(0, 2)));
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 0)));
		} break;

		// cross
		case 11:
		{
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 3)));
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
		} break;

		// snake
		case 12:
		{
			walls.push_back(new Wall(grid->getSquare(1, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 3)));
		} break;

		// T-block
		case 13:
		{
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(1, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(3, 2)));
		} break;

		// long T-block
		case 14:
		{
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(2, 3)));
			walls.push_back(new Wall(grid->getSquare(3, 3)));
			walls.push_back(new Wall(grid->getSquare(1, 3)));
		} break;

		// 2x2 block
		case 15:
		{
			walls.push_back(new Wall(grid->getSquare(1, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 1)));
			walls.push_back(new Wall(grid->getSquare(2, 2)));
			walls.push_back(new Wall(grid->getSquare(1, 2)));
		} break;
	}

	for(unsigned int i = 0; i < walls.size(); i++)
		walls[i]->updateBattlements();
}

void Cursor::clearGrid()
{
	for(unsigned int i = 0; i < walls.size(); i++)
	{
		walls[i]->getRoot()->removeAllChildren();
		walls[i]->getRoot()->detachAllObjects();
		walls[i]->getRoot()->setVisible(false);
		walls[i]->getSquare()->setStructure(NULL, 1);
	}
	walls.clear();
	
	if(cannon != NULL)
	{
		cannon->getRoot()->removeAllChildren();
		cannon->getRoot()->detachAllObjects();
		cannon->getRoot()->setVisible(false);
		cannon->getSquare()->setStructure(NULL, 3);
		cannon = NULL;
	}	
}
