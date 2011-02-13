#ifndef RAMPARTED_LOADPROGRESS
#define RAMPARTED_LOADPROGRESS

#include <string>

namespace Ogre
{
	class Overlay;
	class OverlayContainer;
	class TextAreaOverlayElement;
}

/// Renders simple load progress bar.
class LoadProgress
{
public:

	/// Create a new instance.
	/// @param steps How many times setProgress will be called (used to calculate correct percentage)
	LoadProgress(int steps);

	/// Destroys the instance (and hides the overlay).
	~LoadProgress();

	/// Progress forward.
	/// This will increase the percentage correct amount and will set the given description.
	/// @param description Text to be shown.
	void setProgress(std::string description);

private:

	Ogre::Overlay* loadProgressOverlay;
	Ogre::OverlayContainer* loadProgressBarPanel;
	Ogre::TextAreaOverlayElement* percentageTextArea;
	Ogre::TextAreaOverlayElement* infoTextArea;
	int steps;
	int currentStep;
};

#endif
