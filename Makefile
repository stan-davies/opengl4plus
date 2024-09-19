OBJS = main.cpp

CC = g++

INCLUDE_PATHS = -I include

LIBRARY_PATHS = -L ./

# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -w

LINKER_FLAGS = -lmingw32 -lopengl32 -lglew32 -lglfw3

OBJ_NAME = program

all : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)