#ifndef RAMPARTED_UTILITY
#define RAMPARTED_UTILITY

#include <string>

namespace Ogre
{
	class Vector3;
}

/// Generates a new unique ID string for and Ogre node or entity.
/// @return The next integer as a string.
std::string generateId();

/// Rotate a Ogre vector clockwise 90 degrees around the y-axis.
/// @param vect the vector to rotate.
/// @param turns number of times to rotate.
/// @return the rotated vector.
Ogre::Vector3 rotateClockwise(Ogre::Vector3 vect, unsigned int turns = 1);

/// Perform a circular right shift on an unsigned byte.
/// @param amount distance to shift.
/// @param byte the byte to shift.
/// @return the shifted byte.
unsigned char shiftRight(unsigned int amount, unsigned char byte);

#endif
