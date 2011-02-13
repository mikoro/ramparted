#include "Console.h"
#include "GameEngine.h"

#include <Ogre.h>
#include <OGRE/OgreTextAreaOverlayElement.h>

// some constants to eliminate magic values ;)
const int maxTextAreas = 20;
const int consoleHeight = 350;
const int inputAreaHeight = 23;
const int borderHeight = 2;
const int characterWidth = 8;
const int textAreaXOffset = 6;
const int textAreaYOffset = 6;
const int inputTextAreaYOffset = 12;
const int cursorXOffset = -1;
const int cursorYOffset = 19;
const int lineHeight = 16;

Console::Console(double speed, std::string prefix)
{
	consoleOverlay = 0;
	backgroundPanel = 0;
	inputBackgroundPanel = 0;
	cursorPanel = 0;
	borderPanel = 0;
	inputTextArea = 0;
	this->prefix = prefix;
	inputLine = "";
	completionPart = "";
	completionIndex = 0;
	enabled = false;
	visible = false;
	lineOffset = 0;
	historyIndex = -1;
	cursorIndex = 0;
	position = 0;
	previousPosition = 0;
	windowWidth = 0;
	windowHeight = 0;
	this->speed = speed;
}

void Console::initialize()
{
	// get the predefined overlay and panel definitions
	consoleOverlay = Ogre::OverlayManager::getSingleton().getByName("ConsoleOverlay");
	backgroundPanel = consoleOverlay->getChild("ConsoleBackgroundPanel");
	inputBackgroundPanel = (Ogre::OverlayContainer*)(backgroundPanel->getChild("ConsoleInputBackgroundPanel"));
	cursorPanel = (Ogre::OverlayContainer*)(inputBackgroundPanel->getChild("ConsoleCursorPanel"));
	borderPanel = (Ogre::OverlayContainer*)(backgroundPanel->getChild("ConsoleBorderPanel"));
	
	// insert text areas to correct positions
	for(int i=0; i<maxTextAreas; ++i)
	{
		Ogre::TextAreaOverlayElement* textArea = static_cast<Ogre::TextAreaOverlayElement*>(Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "ConsoleTextArea" + Ogre::StringConverter::toString(i)));
		textArea->setMetricsMode(Ogre::GMM_PIXELS);
		textArea->setPosition(textAreaXOffset, i * lineHeight + textAreaYOffset);
		textArea->setCharHeight(16);
		textArea->setFontName("DejaVuSansMono");
		backgroundPanel->addChild(textArea);
		textAreas.insert(textAreas.begin(), textArea);
	}
	
	// input text area is positioned a little differently
	inputTextArea = static_cast<Ogre::TextAreaOverlayElement*>(Ogre::OverlayManager::getSingleton().createOverlayElement("TextArea", "ConsoleInputTextArea"));
	inputTextArea->setMetricsMode(Ogre::GMM_PIXELS);
	inputTextArea->setPosition(textAreaXOffset, maxTextAreas * lineHeight + inputTextAreaYOffset);
	inputTextArea->setCharHeight(16);
	inputTextArea->setFontName("DejaVuSansMono");
	backgroundPanel->addChild(inputTextArea);
}

void Console::logicTick(double timeStep)
{
	if(enabled)
	{
		int maxLines = lines.size();
		
		// update visible lines, set to blank if not enough lines to show
		for(int i=0; i<maxTextAreas; ++i)
		{
			if(i < (maxLines - lineOffset))
			{
				textAreas[i]->setCaption(lines[i+lineOffset].text);
				textAreas[i]->setColour(lines[i+lineOffset].color);
			}
			else
				textAreas[i]->setCaption("");
		}
		
		// set the input area text
		inputTextArea->setCaption(prefix + inputLine);
		
		// draw the cursor at right position
		cursorPanel->setPosition(textAreaXOffset + prefix.length() * characterWidth + cursorIndex * characterWidth + cursorXOffset, cursorYOffset);
		
		// console should be always visible if enabled
		visible = true;
	}
	
	// save the previous position for interpolation and assign new value
	previousPosition = position;
	position += enabled ? speed * timeStep : -speed * timeStep;
	
	// limit possbile values for position in the range [0,1]
	if(position > 1.0)
		position = 1.0;
	
	if(position < 0)
	{
		position = 0;
		visible = false; // set visible to false when the console could not be seen anymore (scrolled up enough)
	}
	
	visible ? consoleOverlay->show() : consoleOverlay->hide();
}

void Console::renderTick(double interpolation, double)
{
	// interpolate the position
	double interpolatedPosition = 1.0 - (previousPosition * (1.0 - interpolation) + position * interpolation);
	
	// simple trick to make the motion not linear (add terms to make more edgy)
	double tempPosition = interpolatedPosition * interpolatedPosition * interpolatedPosition;
	
	// set the scroll value, ogre needs a value between [0,1], so we need to take window height into account
	consoleOverlay->setScroll(0, (2.0 * consoleHeight / windowHeight) * tempPosition);
}

void Console::shutdown()
{
}

void Console::keyPressed(const OIS::KeyEvent& event)
{
	if(event.key == OIS::KC_F1)
		enabled = !enabled;
	
	if(enabled)
	{
		switch(event.key)
		{
			// remove one character
			case OIS::KC_BACK:
			{
				if(cursorIndex > 0)
				{
					inputLine = inputLine.erase(cursorIndex-1, 1);
					cursorIndex--;
				}
				
				completionPart = inputLine;
				completionIndex = 0;
				
			} break;
			
			// remove one character
			case OIS::KC_DELETE:
			{
				if(cursorIndex < (int)inputLine.length())
					inputLine = inputLine.erase(cursorIndex, 1);
				
				completionPart = inputLine;
				completionIndex = 0;
				
			} break;
			
			// parse the input line and execute command
			case OIS::KC_RETURN:
			{
				// do nothing if empty line
				if(inputLine.length() == 0)
					break;
				
				historyIndex = -1;
				cursorIndex = 0;
				lineOffset = 0;
				completionIndex = 0;
				completionPart = "";
					
				// update the history, ignore duplicates
				if(history.size() == 0)
					history.insert(history.begin(), inputLine);
				else if(history[0] != inputLine)
					history.insert(history.begin(), inputLine);
				
				// break the input line into parts
				std::vector<std::string> parts;
				while(1)
				{
					// trim spaces from the beginning
					while(inputLine.length() > 0 && inputLine[0] == ' ')
						inputLine = inputLine.substr(1);
					
					if(inputLine.length() == 0)
						break;
					
					std::string part;
					
					// if first character is quote, part ends at next quote or line end
					if(inputLine[0] == '"')
					{
						unsigned int nextQuotePosition = inputLine.find("\"", 1);
						
						if(nextQuotePosition != std::string::npos)
						{
							part = inputLine.substr(1, nextQuotePosition-1);
							inputLine = inputLine.substr(nextQuotePosition+1);
						}
						else
						{
							part = inputLine.substr(1);
							inputLine.clear();
						}
					}
					// else it's a character, and part ends at next space or line end
					else
					{
						unsigned int nextSpacePosition = inputLine.find(" ");
						
						if(nextSpacePosition != std::string::npos)
						{
							part = inputLine.substr(0, nextSpacePosition);
							inputLine = inputLine.substr(nextSpacePosition+1);
						}
						else
						{
							part = inputLine;
							inputLine.clear();
						}
					}
					
					parts.push_back(part);
				}
				
				if(parts.size() > 0)
				{
					std::string commandName = parts[0];
					parts.erase(parts.begin());
					
					// execute the command, if any
					if(commands.find(commandName) != commands.end())
					{
						if(!commands[commandName].callback(parts))
							addWarningLine(commandName + ": invalid arguments, usage: " + commands[commandName].usage);
					}
					else
						addWarningLine(commandName + ": unknown command (type help for available commands)");
				}
				
				inputLine = "";
				
			} break;
			
			// move backwards in the history
			case OIS::KC_UP:
			{
				if(history.size() > 0)
				{
					if(historyIndex < (int)history.size() - 1)
						historyIndex++;
					
					inputLine = history[historyIndex];
					completionPart = inputLine;
					completionIndex = 0;
					cursorIndex = inputLine.length();
				}
				
			} break;
			
			// move forwards in the history
			case OIS::KC_DOWN:
			{
				if(historyIndex > -1)
						historyIndex--;
				
				inputLine = (history.size() > 0 && historyIndex >= 0) ? history[historyIndex] : "";
				completionPart = inputLine;
				completionIndex = 0;
				cursorIndex = inputLine.length();
				
			} break;
			
			// move the cursor left
			case OIS::KC_LEFT:
			{
				if(cursorIndex > 0)
					cursorIndex--;
				
			} break;
			
			// move the cursor right
			case OIS::KC_RIGHT:
			{
				if(cursorIndex < (int)inputLine.length())
					cursorIndex++;
				
			} break;
			
			// scroll lines upward
			case OIS::KC_PGUP:
			{
				if(((int)lines.size() - lineOffset) > maxTextAreas)
					lineOffset++;
				
			} break;
			
			// scroll lines downward
			case OIS::KC_PGDOWN:
			{
				if(lineOffset > 0)
					lineOffset--;
				
			} break;
			
			// move cursor to beginning
			case OIS::KC_HOME:
			{
				cursorIndex = 0;
				
			} break;
			
			// move cursor to end
			case OIS::KC_END:
			{
				cursorIndex = inputLine.length();
				
			} break;
			
			// do irssi-style auto completion
			// the idea with completionPart and completionIndex is that user can scroll through all the matches
			//  so that the inputLine changes - we must keep in safe the original input from the user before completion
			case OIS::KC_TAB:
			{
				std::vector<std::string> matches;
				
				// trim spaces from the beginning
				while(completionPart.length() > 0 && completionPart[0] == ' ')
					completionPart = completionPart.substr(1);
				
				// trim spaces from the end
				while(completionPart.length() > 0 && completionPart[completionPart.length()-1] == ' ')
					completionPart = completionPart.substr(0, completionPart.length()-1);
				
				// find all the commands that have the same beginning as the inputted command
				for(std::map<std::string, ConsoleCommand>::iterator i = commands.begin(); i != commands.end(); ++i)
				{
					if(i->first.find(completionPart) == 0)
						matches.push_back(i->first);
				}
				
				// if found any, scroll through the matches is alphabetical order
				// there's a little bug when changing direction - didn't figure out any _simple_ way of fixing it
				if(matches.size() > 0)
				{
					std::sort(matches.begin(), matches.end());
					
					if(GameEngine::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LSHIFT))
						--completionIndex;
						
					if(completionIndex < 0)
					{
						if(matches.size() > 0)
							completionIndex = matches.size() - 1;
						else
							completionIndex = 0;
					}
					
					inputLine = matches[completionIndex] + " ";
					cursorIndex = inputLine.length();
					
					if(!GameEngine::getSingletonPtr()->getKeyboard()->isKeyDown(OIS::KC_LSHIFT))
						++completionIndex;
						
					if(completionIndex >= (int)matches.size())
						completionIndex = 0;
				}
				
			} break;
			
			default:
			{
				// add normal characters, at the moment only basic ascii caharacters work (limitation of the OIS?)
				if(event.text >= 32 && event.text < 128)
				{
					std::string character;
					character += (char)event.text;
					inputLine = inputLine.insert(cursorIndex, character);
					completionPart = inputLine;
					completionIndex = 0;
					cursorIndex++;
				}
				
			} break;
		}
	}
}

void Console::addCommand(ConsoleCommand command)
{
	commands[command.name] = command;
}

void Console::addLine(ConsoleLine line)
{
	lines.insert(lines.begin(), line);
}

void Console::addErrorLine(std::string text)
{
	addLine(ConsoleLine(text, Ogre::ColourValue(1, 0, 0)));
}

void Console::addWarningLine(std::string text)
{
	addLine(ConsoleLine(text, Ogre::ColourValue(1, 1, 0)));
}

void Console::addInfoLine(std::string text)
{
	addLine(ConsoleLine(text, Ogre::ColourValue(0, 1, 0)));
}

void Console::addNormalLine(std::string text)
{
	addLine(ConsoleLine(text, Ogre::ColourValue(1, 1, 1)));
}

void Console::setWindowWidth(int width)
{
	windowWidth = width;
	
	// change the panel widths to match the screen width
	backgroundPanel->setDimensions(windowWidth, consoleHeight);
	inputBackgroundPanel->setDimensions(windowWidth, inputAreaHeight);
	borderPanel->setDimensions(windowWidth, borderHeight);
}

void Console::setWindowHeight(int height)
{
	windowHeight = height;
}

bool Console::isEnabled() const
{
	return enabled;
}

bool Console::consoleCommandHelp(std::vector<std::string>)
{
	// print to console all the commands with their descriptions
	for(std::map<std::string, ConsoleCommand>::iterator i = commands.begin(); i != commands.end(); ++i)
		addInfoLine(i->second.name + ": " + i->second.description);
	
	return true;
}

bool Console::consoleCommandArguments(std::vector<std::string> args)
{
	// print to console all the arguments with their position index
	if(args.size() > 0)
	{
		std::string result;
		
		for(unsigned int i=0; i<args.size(); ++i)
			result += Ogre::StringConverter::toString(i) + ": \"" + args[i] + "\" ";
		
		addInfoLine("arguments: " + result);
		return true;
	}
	else
		return false;
}

bool Console::consoleCommandConsolePrefix(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		prefix = args[0];
		return true;
	}
	else
		return false;
}

bool Console::consoleCommandConsoleSpeed(std::vector<std::string> args)
{
	if(args.size() == 1)
	{
		std::stringstream ss(args[0]);
		ss >> speed;
		
		if(speed < 0)
			speed = 0;
			
		return true;
	}
	else
		return false;
}

void Console::messageLogged(const Ogre::String& message, Ogre::LogMessageLevel lml, bool, const Ogre::String&)
{
	// redirect ogre log messages so that critical messages are printed in red
	switch(lml)
	{
		case Ogre::LML_TRIVIAL: addNormalLine(message); break;
		case Ogre::LML_NORMAL: addNormalLine(message); break;
		case Ogre::LML_CRITICAL: addErrorLine(message); break;
		default: break;
	}
}
