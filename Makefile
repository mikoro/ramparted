# a simple Makefile for compiling Ramparted on linux
# caveats: doesn't detect changes in header files and doesn't have a clue about dependencies...
#
# make				compile Ramparted
# make run			compile and run Ramparted
# make clean		remove object files and executable
# make doc			create documentation with doxygen

# add new source files here, remember the backslash!
SOURCES=src/ActionManager.cpp \
src/BasicCannon.cpp \
src/BasicCannonBall.cpp \
src/Castle.cpp \
src/Console.cpp \
src/Cursor.cpp \
src/FpsCounter.cpp \
src/GameEngine.cpp \
src/GameState.cpp \
src/InterpolationAffector.cpp \
src/IntroGameState.cpp \
src/LoadProgress.cpp \
src/Main.cpp \
src/MenuGameState.cpp \
src/ParticlePool.cpp \
src/Player.cpp \
src/PlayGameState.cpp \
src/SoundManager.cpp \
src/Square.cpp \
src/Utility.cpp \
src/Wall.cpp \
src/World.cpp

PACKAGES=OGRE OIS
# select one, first is debug configuration, second is release configuration
#CFLAGS=-std=c++98 -Wall -ggdb3 -Iinclude `pkg-config $(PACKAGES) --cflags`
CFLAGS=-std=c++98 -Wall -O3 -Iinclude `pkg-config $(PACKAGES) --cflags`
LDFLAGS=`pkg-config $(PACKAGES) --libs` -lfmodex -lEditableTerrain
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=ramparted

all: $(EXECUTABLE)

run: $(EXECUTABLE)
	@echo "Launching $(EXECUTABLE)"
	@cd bin; ./$(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	@echo "Linking $(EXECUTABLE)"
	@g++ $(LDFLAGS) $(OBJECTS) -o bin/$@

.cpp.o:
	@echo "Compiling $<"
	@g++ -c $(CFLAGS) $< -o $@

clean:
	@echo "Removing files"
	@rm -f $(OBJECTS) bin/$(EXECUTABLE)

doc:
	@echo "Creating documentation"
	@doxygen Doxyfile
