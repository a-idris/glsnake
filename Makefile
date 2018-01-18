LIBDIRS= -L/usr/X11R6/lib
LDLIBS = -lglut -lGL -lGLU -lX11 -lm 

CPPFLAGS= -O3 -g
LDFLAGS= $(CPPFLAGS) $(LIBDIRS)

CXX = g++

OBJECTS=snake.o game_logic.o

snake: $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) -o snake $(LDLIBS)

snake.o: snake.h game_logic.h

game_logic.o: game_logic.h

