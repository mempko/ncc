include ../../library/config 
include config 
SOURCES= example1.cpp example2.cpp example3.cpp example4.cpp example5.cpp example6.cpp testgame.cpp rungame.cpp

OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLES = $(SOURCES:.cpp=)

LDFLAGS=-lncc -lboost_filesystem -lboost_iostreams -lboost_serialization -llua5.1 -losg -losgViewer -losgText -losgTerrain -losgGA -losgFX -losgDB -losgSim -losgUtil -losgParticle -lode -lluabindd -lalut 
LIBDIRS=-L../../library/lib -L/usr/lib -L/usr/local/lib

ARGS=-O3
all: $(EXECUTABLES) 

#%.exe: %.cpp
#	$(CXX) -L$(NCC_LIB) $< -o $@ 

%:%.cpp
	$(CXX) $(ARGS) -I$(INCLUDES) -I$(NCC_PATH) -I$(LUA_PATH) -I$(ALTINCLUDES) $(LIBDIRS) $< -o $@ $(LDFLAGS)
clean:
	rm -f $(EXECUTABLES) 



