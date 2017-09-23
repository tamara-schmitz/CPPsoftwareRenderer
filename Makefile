#OBJS specifies which files to compile as part of the project
OBJS = src/*.cpp src/*/*.cpp

#CXX specifies which compiler we're using
CXXWIN64   = x86_64-w64-mingw32-g++
ifdef ($(CXX))
	CXXLINUX64 = $(CXX)
else
	CXXLINUX64 = g++
endif

#INCLUDE_PATHS specifies the additional include paths we'll need
INCLUDE_PATHS = -Iinclude -Isrc -Iinclude/vmath-0.12

#LIBRARY_PATHS specifies the additional library paths we'll need
LIBRARY_PATHS = -Linclude/i686-w64-mingw32/lib

#COMPILER_FLAGS specifies the additional compilation options we're using
# -Wl,-subsystem,windows gets rid of the console window
COMPILER_FLAGS_WIN = -Wall -Werror -pedantic-errors -Wl,-subsystem,windows -march=core2 -O3 -fomit-frame-pointer -std=c++11 -static-libgCXX -static-libstdc++
COMPILER_FLAGS_LINUX = -Wall -Werror -pedantic-errors -march=core2 -O3 -std=c++11

# DEBUG_FLAGS added in addition to COMPILER_FLAGS
DEBUG_FLAGS = -DPRINT_DEBUG_STUFF -g

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS_WIN =  -Wl,-Bstatic -lmingw32 -lwinpthread -lpthread -lSDL2main -Wl,-Bdynamic -lSDL2
LINKER_FLAGS_LINUX = -lSDL2

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME_WIN64 = build/SDLsoftwarerenderer_win64.exe
OBJ_NAME_LINUX64 = build/SDLsoftwarerenderer_linux64

#windows
win64 : $(OBJS)
	$(CXXWIN64) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_WIN) $(LINKER_FLAGS_WIN) -o $(OBJ_NAME_WIN64)
win64-debug : $(OBJS)
	$(CXXWIN64) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_WIN) $(DEBUG_FLAGS) $(LINKER_FLAGS_WIN) -o $(OBJ_NAME_WIN64)

#linux
linux64 : $(OBJS)
	$(CXX) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_LINUX) $(LINKER_FLAGS_LINUX) -o $(OBJ_NAME_LINUX64)
linux64-debug : $(OBJS)
	$(CXX) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_LINUX) $(DEBUG_FLAGS) $(LINKER_FLAGS_LINUX) -o $(OBJ_NAME_LINUX64)
# default
.DEFAULT_GOAL := linux64
