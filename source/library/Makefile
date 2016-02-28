include config 
SOURCES = \
	src/controller/controller_manager.cpp \
	src/controller/message.cpp \
	src/controller/message_manager.cpp \
	src/elements/id.cpp \
	src/object/object_manager.cpp \
	src/object/object_utilities.cpp \
	src/object/ode/ode_manager.cpp \
	src/object/ode/ode_policies.cpp \
	src/object/osg/osg_manager.cpp \
	src/object/osg/osg_policies.cpp \
	src/object/osg_ode/osg_ode.cpp \
	src/scripting/script.cpp \
	src/scripting/script_controller.cpp \
	src/scripting/script_utilities.cpp \
	src/sound/oal_manager.cpp \
	src/utilities/unicode.cpp 

OBJECTS = $(SOURCES:.cpp=.o)

TARGET = lib/libncc.a
#ARGS=-pg
ARGS=-O3
all:	$(TARGET)

$(TARGET):	$(OBJECTS)
	$(AR) rcu $@ $(OBJECTS) 
	ranlib $@

%.o:%.cpp
	$(CXX) -ftemplate-depth-100 $(ARGS)  -c $? -o $@ -I- -I$(INCLUDES) -I$(LUA_PATH) -I$(LUABIND_PATH) -I$(ALTINCLUDES) 
clean:
	rm -f $(OBJECTS) $(TARGET)


