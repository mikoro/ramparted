#include "Utility.h"

#include <algorithm>
#include <OGRE/OgreVector3.h>
#include <string>
#include <sstream>

std::string generateId() {
	static int counter = 0;
	std::stringstream aux;
	aux << counter;
	counter++;
	return aux.str();
}

Ogre::Vector3 rotateClockwise(Ogre::Vector3 vect, unsigned int turns)
{
	for(unsigned int t = 0; t < turns; t++)
	{
		vect.z = - vect.z;
		std::swap(vect.x, vect.z);
	}
	return vect;
}

unsigned char shiftRight(unsigned int amount, unsigned char byte)
{
	for(unsigned int n = 0; n < amount; n++)
	{
		unsigned char newMSB = (0x01 & byte) << 7;
		byte >>= 1;
		byte |= newMSB;
	}
	return byte;
}
