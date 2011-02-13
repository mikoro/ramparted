#include "LoadProgress.h"
#include "GameEngine.h"

#include <Ogre.h>
#include <OGRE/OgreTextAreaOverlayElement.h>

const int loadProgressBarWidth = 536;

LoadProgress::LoadProgress(int steps)
{
	this->steps = steps;
	currentStep = 0;
	
	loadProgressOverlay = Ogre::OverlayManager::getSingleton().getByName("LoadProgressOverlay");
	Ogre::OverlayContainer* loadProgressPanel = loadProgressOverlay->getChild("LoadProgressPanel");
	Ogre::OverlayContainer* loadProgressBackgroundPanel = (Ogre::OverlayContainer*)(loadProgressPanel->getChild("LoadProgressBackgroundPanel"));
	loadProgressBarPanel = (Ogre::OverlayContainer*)(loadProgressBackgroundPanel->getChild("LoadProgressBarPanel"));
	percentageTextArea = (Ogre::TextAreaOverlayElement*)(loadProgressBackgroundPanel->getChild("LoadProgressPercentageTextArea"));
	infoTextArea = (Ogre::TextAreaOverlayElement*)(loadProgressBackgroundPanel->getChild("LoadProgressInfoTextArea"));
	
	loadProgressOverlay->show();
}

LoadProgress::~LoadProgress()
{
	loadProgressOverlay->hide();
}

void LoadProgress::setProgress(std::string description)
{
	++currentStep;
	double percentage = currentStep / (double)steps;
	loadProgressBarPanel->setWidth((int)(loadProgressBarWidth * percentage));
	percentageTextArea->setCaption(Ogre::StringConverter::toString((int)(percentage * 100)) + " %");
	infoTextArea->setCaption(description);
	GameEngine::getSingleton().doExtraRendering();
}
