#include "Cannon.h"
#include "BasicCannon.h"
#include "BasicCannonBall.h"
#include "Cursor.h"
#include "Player.h"
#include "Square.h"
#include "Wall.h"
#include "World.h"
#include "GameEngine.h"
#include "PlayGameState.h"
#include <ETTerrainInfo.h>
#include <ETTerrainManager.h>

#include <algorithm>
#include <Ogre.h>

#ifndef G
#define G 1500
#endif

const double fireDelay = 0.1;

Player::Player(World* world, std::string name)
{
	this->world = world;
	this->name = name;
	score = 0;
	cursor = new Cursor(this, world);
	canFire = true;
}

void Player::addWall(Wall* wall)
{
	wall->setOwner(this);
	walls.push_back(wall);
}

void Player::addCannon(Cannon* cannon)
{
	if(cannon->getOwner() != this)
		cannon->setOwner(this);
	if(std::count(cannons.begin(), cannons.end(), cannon) == 0)
		cannons.push_back(cannon);
}

void Player::requestFire()
{
	if(!canFire)
		return;

	std::vector<Cannon*> readyCannons;

	for(unsigned int i=0; i<cannons.size(); ++i)
		if(cannons[i]->isReady() && !cannons[i]->isDestroyed())
			readyCannons.push_back(cannons[i]);

	if(readyCannons.size() > 0)
	{
		int cannonNumber = rand() % readyCannons.size();
		readyCannons[cannonNumber]->fire();
		canFire = false;
	}
}

void Player::allowFiring()
{
	canFire = true;
}

void Player::updateWalls()
{
	for(unsigned int i = 0; i < walls.size(); i++)
		walls[i]->updateBattlements();
}

void Player::updateCannons(double timeStep)
{
	ET::TerrainManager* terrainManager = static_cast<PlayGameState*>(GameEngine::getSingletonPtr()->getState("play"))->getTerrainManager();
	Ogre::Real oceanLevel = static_cast<PlayGameState*>(GameEngine::getSingletonPtr()->getState("play"))->getOceanLevel();

	// Rotate cannons
	for(std::vector<Cannon*>::iterator it = cannons.begin(); it != cannons.end(); ++it)
	{
		Ogre::Quaternion standOrientation;
		Ogre::Quaternion tubeOrientation;

		Ogre::Vector3 cursorPos(cursor->getPosition());
		Ogre::Vector3 rootPos((*it)->getTube()->getWorldPosition());
		Ogre::Radian yaw = Ogre::Math::ATan2(cursorPos.x - rootPos.x, cursorPos.z - rootPos.z);
		standOrientation = Ogre::Quaternion(yaw, Ogre::Vector3::UNIT_Y);

		Ogre::Real velocity = (*it)->getInitialVelocity();
		Ogre::Real distance = (Ogre::Vector2(cursorPos.x - rootPos.x, cursorPos.z - rootPos.z)).length();		
		
		Ogre::Real sqrVelocity = Ogre::Math::Sqr(velocity);
		Ogre::Real ydiff = terrainManager->getTerrainInfo().getHeightAt(cursorPos.x, cursorPos.z) - rootPos.y;
		Ogre::Real underRoot = Ogre::Math::Sqr(sqrVelocity) - G * (G * Ogre::Math::Sqr(distance) + 2 * ydiff * sqrVelocity);
		
		if(underRoot >= 0 && distance != 0.0)
		{
			Ogre::Radian pitch = Ogre::Math::ATan((sqrVelocity + Ogre::Math::Sqrt(underRoot))/(G * distance));
			tubeOrientation = Ogre::Quaternion(pitch, Ogre::Vector3::NEGATIVE_UNIT_X);
		}
		else
		{
			tubeOrientation = (*it)->getTube()->getOrientation();
		}
		
		// Translate cannonballs
		// TODO This should be also interpolated
		if(!(*it)->getBall()->isReady())
		{
			Ogre::Vector3 v = (*it)->getBall()->getVelocity();
			v.y -= G * timeStep;
			(*it)->getBall()->setVelocity(v);
			(*it)->getBall()->getRoot()->translate(v * timeStep, Ogre::Node::TS_WORLD);
			
			Ogre::Vector3 ballPos = (*it)->getBall()->getRoot()->getWorldPosition();
			if(ballPos.y <= oceanLevel || ballPos.y <= terrainManager->getTerrainInfo().getHeightAt(ballPos.x, ballPos.z))
			{
				(*it)->getBall()->hit();
			}
		}
		static_cast<BasicCannon*>(*it)->update(standOrientation, tubeOrientation);
	}
}

void Player::renderCannons(double interpolation, double timeStep)
{
	// Render the mesh animations of the cannons
	for(std::vector<Cannon*>::iterator it = cannons.begin(); it != cannons.end(); ++it)
	{
		if((*it)->getAnim()->hasEnded())
		{
			(*it)->getAnim()->setTimePosition(0);
			(*it)->getAnim()->setEnabled(false);
		}
		else if((*it)->getAnim()->getEnabled())
		{
			(*it)->getAnim()->addTime(timeStep);
		}

		(static_cast<BasicCannon*>(*it))->render(interpolation);
	}
}

void Player::removeDeadWalls()
{
	for(unsigned int i = 0; i < walls.size(); i++)
	{
		if(walls[i]->isDestroyed())
		{
			walls[i]->getRoot()->removeAllChildren();
			walls[i]->getSquare()->getRoot()->removeAllChildren();
			walls[i]->getSquare()->setStructure(NULL, 1);
			walls.erase(walls.begin() + i);
			i--;
		}
	}
}

void Player::clearSquares()
{
	for(int k = 0; k < 48; k++)
	{
		for(int i = 0; i < 48; i++)
		{
			Square* sqr = world->getSquare(i, k);
			
			if(sqr->getOwner() == this)
			{
				sqr->setOwner(NULL);
			}
		}
	}	
}

void Player::clearCannons()
{	
	for(unsigned int i = 0; i < cannons.size(); i++)
	{
		cannons[i]->setOwner(NULL);
	}
	cannons.clear();
}

int Player::claimSquares()
{
	floodMark(world->getSquare(0, 0));
	int claimed = 0;
		
	for(int k = 0; k < 48; k++)
	{
		for(int i = 0; i < 48; i++)
		{
			Square* sqr = world->getSquare(i, k);
			
			if(!(sqr->getOwner() != NULL ||
				 sqr->isMarked() ||
			     sqr->isBoundary() ||
				 sqr->getStructure() != NULL &&
				 sqr->getStructure()->getStructureType() == "Wall"))
			{				
				sqr->setOwner(this);
				claimed++;
			}

			sqr->mark(false);
		}
	}

	return claimed;
}

std::string  Player::getName() { return name; }
int  Player::getScore() { return score; }
Cursor*  Player::getCursor() { return cursor; }
std::vector<Cannon*>  Player::getCannons() { return cannons; }

void Player::floodMark(Square* sqr)
{
	if(sqr == NULL) return;
	if(sqr->isMarked()) return;
		
	if(sqr->getStructure() != NULL &&
	   !sqr->getStructure()->isDestroyed() &&
	   sqr->getStructure()->getOwner() == this &&
	   sqr->getStructure()->getStructureType() == "Wall")
	{
		return;
	}
	
	sqr->mark(true);
	
	floodMark(sqr->getAdjacent(0));
	floodMark(sqr->getAdjacent(1));
	floodMark(sqr->getAdjacent(2));
	floodMark(sqr->getAdjacent(3));
	floodMark(sqr->getAdjacent(4));
	floodMark(sqr->getAdjacent(5));
	floodMark(sqr->getAdjacent(6));
	floodMark(sqr->getAdjacent(7));

	return;
}
