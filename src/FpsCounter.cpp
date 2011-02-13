#include "FpsCounter.h"

// the magic constants ;)
const double refreshInterval = 0.1;
const double reduceFactor = 0.8;

FpsCounter::FpsCounter()
{
	tickCount = 0;
	tickTimeSum = 0;
	previousTickTime = timer.getMicroseconds() / 1000000.0;
	previousRefreshTime = timer.getMicroseconds() / 1000000.0;
	fps = 0;
}

void FpsCounter::tick()
{	
	double currentTime = timer.getMicroseconds() / 1000000.0;
	double tickTime = currentTime - previousTickTime;
	tickTimeSum += tickTime;
	tickCount++;
	previousTickTime = currentTime;
	
	// because we're taking the continuous sum of tick times and counts, they must be
	//  refreshed from time to time - to keep things responsive
	if(currentTime - previousRefreshTime > refreshInterval)
	{
		tickTimeSum *= reduceFactor;
		tickCount *= reduceFactor;
		previousRefreshTime = currentTime;
	}
	
	// frames per second is the inverse of frametime
	fps = 1.0 / (tickTimeSum / (double)(tickCount));
}

std::string FpsCounter::getFps()
{
	// format the double value with std::stringstream
	std::stringstream ss;
	ss << std::fixed << std::setprecision(0) << fps;
	return ss.str();
}
