OBJS = main.cpp

CC = g++

INCLUDE_PATHS = -I include

LIBRARY_PATHS = -L ./

# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w

LINKER_FLAGS = -lmingw32 -lopengl32 -lglew32 -lglfw3

OBJ_NAME = program

all : program versionfile message
# @ hides commands in terminal
# git describe gets version number, --dirty adds -dirty to version number if changes have been made since last commit and tag
versionfile:
	@echo:#define VERSION "$(shell git describe --dirty)"> version.h
program:
	@$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)
message:
	@echo:Don't forget to commit and tag. Latest commit was ${shell git describe}.