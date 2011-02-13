#ifndef RAMPARTED_CONSOLE
#define RAMPARTED_CONSOLE

#include <string>
#include <vector>
#include <OGRE/OgreColourValue.h>
#include <OGRE/OgreLog.h>

namespace OIS
{
	class KeyEvent;
}

namespace Ogre
{
	class Overlay;
	class OverlayContainer;
	class TextAreaOverlayElement;
}

/// Console command callback function pointer.
/// @param args Vector which contains argument strings user inputted.
/// @return False if arguments were invalid, true otherwise.
typedef bool (*ConsoleCommandCallback)(std::vector<std::string> args);

/// Represents one console command.
struct ConsoleCommand
{
	/// Construct empty command.
	ConsoleCommand()
	{
		this->name = "";
		this->description = "";
		this->usage = "";
		this->callback = 0;
	}
	
	/// Construct specific command.
	/// @param name Name of the command.
	/// @param description Description of the command.
	/// @param usage Usage string for the command.
	/// @param callback Callback function of the command.
	ConsoleCommand(std::string name, std::string description, std::string usage, ConsoleCommandCallback callback)
	{
		this->name = name;
		this->description = description;
		this->usage = usage;
		this->callback = callback;
	}
	
	std::string name; 					///< Name of the command.
	std::string description; 			///< Description of the command.
	std::string usage; 					///< Usage string for the command.
	ConsoleCommandCallback callback; 	///< Callback function of the command.
};

/// Represents one console line.
struct ConsoleLine
{
	/// Construct specific line.
	/// @param text Text of the line.
	/// @param color Color of the line.
	ConsoleLine(std::string text, Ogre::ColourValue color)
	{
		this->text = text;
		this->color = color;
	}
	
	std::string text; 			///< Text of the line.
	Ogre::ColourValue color; 	///< Color of the line.
};

/// Simple quake style console.
class Console : public Ogre::LogListener
{
public:
		
	/// Construct a new instance.
	Console(double speed, std::string prefix);
	
	/// Initialize the console.
	/// Must be called before anything else.
	void initialize();
	
	/// Do the logic stuff.
	/// @param timeStep Time since last call.
	void logicTick(double timeStep);
	
	/// Do the rendering.
	/// @param interpolation Interpolation between logic ticks.
	/// @param timeStep Time since last call.
	void renderTick(double interpolation, double timeStep);
	
	/// Frees reserved resources.
	void shutdown();
	
	/// Inject key press events.
	/// @param event OIS keyboard event.
	void keyPressed(const OIS::KeyEvent& event);
	
	/// Add a new command.
	/// @param command ConsoleCommand structure representing the command.
	void addCommand(ConsoleCommand command);
	
	/// Add a new line.
	/// @param line ConsoleLine structure representing the line.
	void addLine(ConsoleLine line);
	
	/// Add an error line.
	/// Color of the line will be red.
	/// @param text Text of the line.
	void addErrorLine(std::string text);
	
	/// Add a warning line.
	/// Color of the line will be yellow.
	/// @param text Text of the line.
	void addWarningLine(std::string text);
	
	/// Add an info line.
	/// Color of the line will be green.
	/// @param text Text of the line.
	void addInfoLine(std::string text);
	
	/// Add a normal line.
	/// Color of the line will be white.
	/// @param text Text of the line.
	void addNormalLine(std::string text);
	
	/// Set the width of the render window.
	/// This will affect only the console rendering, not the window itself.
	/// @param width Width of the window.
	void setWindowWidth(int width);
	
	/// Set the height of the render window.
	/// This will affect only the console rendering, not the window itself.
	/// @param height Height of the window.
	void setWindowHeight(int height);
	
	/// Get the state of the console.
	/// @return True if enabled, false otherwise.
	bool isEnabled() const;
	
	bool consoleCommandHelp(std::vector<std::string> args); ///< Implementation for the console command "help".
	bool consoleCommandArguments(std::vector<std::string> args); ///< Implementation for the console command "arguments".
	bool consoleCommandConsolePrefix(std::vector<std::string> args); ///< Implementation for the console command "consolePrefix".
	bool consoleCommandConsoleSpeed(std::vector<std::string> args); ///< Implementation for the console command "consoleSpeed".
	
private:
		
	/// Callback function for the ogre logging system.
	void messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool maskDebug, const Ogre::String& logName);
	
	Ogre::Overlay* consoleOverlay;
	Ogre::OverlayContainer* backgroundPanel;
	Ogre::OverlayContainer* inputBackgroundPanel;
	Ogre::OverlayContainer* cursorPanel;
	Ogre::OverlayContainer* borderPanel;
	std::vector<Ogre::TextAreaOverlayElement*> textAreas;
	Ogre::TextAreaOverlayElement* inputTextArea;
	std::vector<ConsoleLine> lines;
	std::vector<std::string> history;
	std::map<std::string, ConsoleCommand> commands;
	std::string prefix;
	std::string inputLine;
	std::string completionPart;
	int completionIndex;
	bool enabled;
	bool visible;
	int lineOffset;
	int historyIndex;
	int cursorIndex;
	double position;
	double previousPosition;
	int windowWidth;
	int windowHeight;
	double speed;
};

#endif
