#OBJS specifies which files to compile as part of the project
OBJS = src/*.cpp src/*/*.cpp

#CC specifies which compiler we're using
CC   = i686-w64-mingw32-g++ 
CC64 = x86_64-w64-mingw32-g++

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS   = -Iinclude -Isrc -Iinclude/vmath-0.12

#LIBRARY_PATHS 64 specifies the additional library paths we'll need
LIBRARY_PATHS = -Linclude/i686-w64-mingw32/lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS = -Wall -Wl,-subsystem,windows -march=core2 -O3 -fomit-frame-pointer -fexpensive-optimizations -std=c++11 -static-libgcc -static-libstdc++

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS =  -Wl,-Bstatic -lmingw32 -lwinpthread -lpthread -lSDL2main -Wl,-Bdynamic -lSDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = build/win32/SDLsoftwarerenderer32.exe
OBJ_NAME64 = build/win64/SDLsoftwarerenderer.exe

#32bit
win32 : $(OBJS)
	$(CC) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME)

#64bit
win64 : $(OBJS)
	$(CC64) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS) $(LINKER_FLAGS) -o $(OBJ_NAME64)

# default
win64 := all
