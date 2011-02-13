#include "GameEngine.h"
#include "ConsoleCommandWrappers.h"
#include "IntroGameState.h"

#include <OGRE/OgreException.h>

#ifdef WIN32
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
#else
int main(int, char**)
#endif
{
	srand((unsigned int)time(0));

	new GameEngine();
	GameEngine& engine = GameEngine::getSingleton();
	
	try
	{
		engine.initialize();
		
		#include "ConsoleCommandAdders.h"
		
		engine.pushState(new IntroGameState());
		engine.mainloop();
	}
	catch(Ogre::Exception& e) { std::cout << "An unhandled Ogre exception caught: " << e.getDescription() << std::endl; }
	catch(std::exception& e) { std::cout << "An unhandled STD exception caught: " << e.what() << std::endl; }
	catch(...) { std::cout << "An unknown exception caught" << std::endl; throw; }

	engine.shutdown();
	return 0;
}
