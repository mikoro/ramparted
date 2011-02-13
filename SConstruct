# a "Makefile" for the scons program to compile Ramparted on linux
# dependency graph is automatically created, header changes are detected too and object files are outputted to obj directory
#
# scons				compile Ramparted with release configuration
#
# you may add following arguments:
#
# debug=1			compile with debug configuration
# warn=1			enable extra warnings
# run=1				execute the program after successful compilation
# doc=1				create documentation with doxygen

# add new source files here
sources = Split("""
ActionManager.cpp
BasicCannon.cpp
BasicCannonBall.cpp
Castle.cpp
Console.cpp
Cursor.cpp
FpsCounter.cpp
GameEngine.cpp
GameState.cpp
InterpolationAffector.cpp
IntroGameState.cpp
LoadProgress.cpp
Main.cpp
MenuGameState.cpp
ParticlePool.cpp
Player.cpp
PlayGameState.cpp
SoundManager.cpp
Square.cpp
Utility.cpp
Wall.cpp
World.cpp
""")

for i,v in enumerate(sources):
	sources[i] = "obj/" + v

BuildDir("obj", "src", duplicate = 0)

environment = Environment(CXXCOMSTR = "Compiling $SOURCE", LINKCOMSTR = "Linking $TARGET")
environment.ParseConfig("pkg-config --cflags --libs OGRE OIS")
environment.Append(CCFLAGS = "-std=c++98 -Wall")
environment.Append(CPPPATH = "include")
environment.Append(LIBS = ["fmodex", "EditableTerrain"])

target = environment.Program("bin/ramparted", sources)

debug = ARGUMENTS.get("debug", 0)
warn = ARGUMENTS.get("warn", 0)
run = ARGUMENTS.get("run", 0)
doc = ARGUMENTS.get("doc", 0)

if int(debug):
	environment.Append(CCFLAGS = "-ggdb3")
else:
	environment.Append(CCFLAGS = "-O3")

if int(warn):
	environment.Append(CCFLAGS = "-Wextra")
	
if int(run):
	import os
	environment.Replace(ENV = os.environ)
	environment.AddPostAction(target, Action("cd bin && ./ramparted"))
	AlwaysBuild(target)

if int(doc):
	environment.AddPostAction(target, Action("doxygen Doxyfile"))
	AlwaysBuild(target)

