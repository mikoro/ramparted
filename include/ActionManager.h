#ifndef RAMPARTED_ACTIONMANAGER
#define RAMPARTED_ACTIONMANAGER

#include <map>
#include <OIS/OISKeyboard.h>
#include <OIS/OISMouse.h>
#include <vector>

/// Interface for different action updaters.
class ActionUpdater
{
public:
	
	virtual ~ActionUpdater() {}
	
	/// Get the current state of this action.
	virtual bool getActionState() = 0;
};

/// Implements ActionUpdater for reacting to keyboard.
class KeyboardActionUpdater : public ActionUpdater
{
public:
		
	KeyboardActionUpdater(OIS::KeyCode keyCode, bool enabledWithConsole);
	bool getActionState();
	
private:
	
	OIS::KeyCode keyCode;
	bool enabledWithConsole;
};

/// Implements ActionUpdater for reacting to mouse.
class MouseActionUpdater : public ActionUpdater
{
public:
		
	MouseActionUpdater(OIS::MouseButtonID mouseButton, bool enabledWithConsole);
	bool getActionState();
	
private:
		
	OIS::MouseButtonID mouseButton;
	bool enabledWithConsole;
};

/// Handles actions of players.
class ActionManager
{
public:

	/// Construct a new instance.
	ActionManager();
	
	/// Add a new action.
	/// If an action already exists, it gets overwritten.
	/// @param player Which player should have this action.
	/// @param identifier Unique identifier for GIVEN PLAYER.
	/// @param updater Pointer to a class that implements the ActionUpdater interface (memory is freed by the manager).
	void addAction(int player, std::string identifier, ActionUpdater* updater);
	
	/// Get the state of a given action.
	/// @param player Which player should be searched for the action.
	/// @param identifier Identifier of the action.
	/// @return The state of the action.
	bool getAction(int player, std::string identifier);
	
	/// Update all underlying action with their ActionUpdaters.
	void update();
	
	/// Frees all the pointers given to the manager.
	void shutdown();

private:
	
	/// Contains both the updater and the return value of that updater.
	/// Frees the updater automatically when destroyed.
	struct ActionContainer
	{
		ActionContainer() {}
		ActionContainer(ActionUpdater* updater) : updater(updater), state(false) {}
		~ActionContainer() { delete updater; }
		
		ActionUpdater* updater;
		bool state;
	};
	
	typedef std::map<std::string, ActionContainer*> actionContainerMap;
	std::vector<actionContainerMap> actionContainerMaps;
};

#endif
