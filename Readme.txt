Ramparted 1.0

ABOUT
===============================================================================

This is a "simple" rampart clone using 3D-graphics. We're making this program
as a project work for the course "C/C++ programming" at the Helsinki University
of Technology. Our team consists of three developers:

Teemu Kemppainen
Mikko Ronkainen
Tuomas Seppänen

COMPILING AND RUNNING
===============================================================================

Linux:
	
	You will need these dependencies installed:
	
	- Ogre 1.4.5 (http://downloads.sourceforge.net/ogre/ogre-linux_osx-v1-4-5.tar.bz2)
	- FMOD Ex 4 (http://www.fmod.org/index.php/download)
	- Editable Terrain Manager 2.2 (3rdparty/etm-2.2.tar.gz)
	- Object Oriented Input System 1.0 (3rdparty/ois-1.0.tar.gz)
	
	You should really use your distro's package manager to install Ogre. It has
	a lot of dependencies and other little quirks. But if you're feeling lucky,
	you could try compiling it from source. FMOD is easy to install (no compiling
	required). The last two dependencies probably cannot be found by your packet
	manager (unless using Arch Linux ;), so you have to compile and install them
	manually. 
	
	Now that you're sure all the dependencies are correctly installed you can
	try compiling. Goto the root folder (where Makefile, SConstruct etc. reside)
	and issue:
	
	% make
	
	If you prefer SCons for compiling, try following:
	
	% scons
	
	Now if everything went smoothly, you're ready to play. Goto the bin folder,
	edit the rampated.ini file to suit your needs and launch the game:
	
	% cd bin
	% vi ramparted.ini
	% ./ramparted
	
	If you had any problems with compiling, check the troubleshoot section.
	
	If you're compiling on HUT's linux machines, there is some special magic
	you _have_ to do before doing anything else. This is because all the
	dependencies are installed in non-standard locations. To compile with
	make, issue following commands (assuming Bash shell):
	
	% source hut.sh
	% make
	
	To do the same with SCons, try following:
	
	% source hut.sh
	% scons
	
Windows:

	Compiling on Windows is rather easy, all you need is some bandwidth to
	download Microsoft Visual C++ 2008 Express Edition from here:
	
	http://www.microsoft.com/express/download/
	
	After you have finished installing visual studio you have to install Windows
	versions of all the dependencies. Luckily this is very easy, just unpack
	the "windows.rar" package to the project root folder. It contains all the
	correct headers, libraries and dll's.
	
	Now you're ready to compile. Open the "Ramparted.sln" with visual studio
	and select "Release" configuration if not already selected. Then press
	F7 to compile.
	
	After successful compilation, goto the "bin" folder, edit the "ramparted.ini"
	file to suit your needs and double click on the executable to launch the
	program.

PLAYING
===============================================================================

TROUBLESHOOT
===============================================================================

