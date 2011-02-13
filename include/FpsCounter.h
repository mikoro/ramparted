#ifndef RAMPARTED_FPSCOUNTER
#define RAMPARTED_FPSCOUNTER

#include <OGRE/OgreTimer.h>

/// Helper for fps calculation.
class FpsCounter
{
public:
	
	/// Construct a new instance.
	FpsCounter();
	
	/// Update the counter.
	/// This should be called once per every "cycle" of which fps (or ticks per second) is tracked.
	void tick();
	
	/// Get the formatted fps string.
	/// @return Current fps as string.
	std::string getFps();

private:
	
	Ogre::Timer timer;
	double tickCount;
	double tickTimeSum;
	double previousTickTime;
	double previousRefreshTime;
	double fps;
};

#endif
