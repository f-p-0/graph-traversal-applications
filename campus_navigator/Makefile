TARGET = campus_navigator
SRC_FILES = campus_navigator.cpp campus_navigator_functions.cpp weighted_graph.cpp

INC_PATH = 
LIB_PATH = 

CXX = g++
CXXFLAGS_DEBUG = -O2 -g
CXXFLAGS_WARN = -Wall -Wextra -Wunreachable-code -Wshadow -Wpedantic
CPPVERSION = -std=c++17

LIBS = -lsfml-graphics -lsfml-window -lsfml-system

OBJECTS = $(SRC_FILES:.cpp=.o)

ifeq ($(shell echo "Windows"), "Windows")
	TARGET := $(TARGET).exe
	DEL = del
	Q=
else
	DEL = rm -f
	Q="
endif

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) -o $@ $^ $(if $(LIB_PATH),-L$(LIB_PATH)) $(LIBS)

.cpp.o:
	$(CXX) $(CPPVERSION) $(CXXFLAGS_DEBUG) $(CXXFLAGS_WARN) $(if $(INC_PATH),-I$(INC_PATH)) -o $@ -c $<

clean:
	$(DEL) $(TARGET) $(OBJECTS) Makefile.bak

depend:
	@sed -i.bak '/^# DEPENDENCIES/,$$d' Makefile
	@$(DEL) sed*
	@echo $(Q)# DEPENDENCIES$(Q) >> Makefile
	@$(CXX) $(if $(INC_PATH),-I$(INC_PATH)) -MM $(SRC_FILES) >> Makefile

.PHONY: all clean depend