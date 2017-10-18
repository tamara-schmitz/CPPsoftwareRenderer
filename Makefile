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
COMPILER_FLAGS_COMMON = -Wall -pedantic-errors -std=c++11 -Wno-unused-variable -fstack-protector-strong
COMPILER_FLAGS_RELEASE = -O3 -fomit-frame-pointer -flto -fwhole-program
COMPILER_FLAGS_DEBUG = -g -fno-omit-frame-pointer -fsanitize=address,undefined -fstack-check -fuse-ld=gold

COMPILER_FLAGS_WIN = $(COMPILER_FLAGS_COMMON) -Wl,-subsystem,windows -march=core2 -static-libgCXX -static-libstdc++
COMPILER_FLAGS_LINUX = $(COMPILER_FLAGS_COMMON) -march=core2

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS_WIN =  -Wl,-Bstatic -lmingw32 -lwinpthread -lpthread -lSDL2main -Wl,-Bdynamic -lSDL2
LINKER_FLAGS_LINUX = -lSDL2 -lpthread

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME_PREFIX = build/SDLsoftwarerenderer_

clean :
	rm build/*

#windows
win64 : $(OBJS)
	$(CXXWIN64) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_WIN) $(COMPILER_FLAGS_RELEASE) $(LINKER_FLAGS_WIN) -o $(OBJ_NAME_PREFIX)win64
win64-debug : $(OBJS)
	$(CXXWIN64) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_WIN) $(COMPILER_FLAGS_DEBUG) $(LINKER_FLAGS_WIN) -o $(OBJ_NAME_PREFIX)win64-debug

#linux
linux64 : $(OBJS)
	$(CXX) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_LINUX) $(COMPILER_FLAGS_RELEASE) $(LINKER_FLAGS_LINUX) -fprofile-generate -o $(OBJ_NAME_PREFIX)linux64
	$(OBJ_NAME_PREFIX)linux64 -t
	$(CXX) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_LINUX) $(COMPILER_FLAGS_RELEASE) $(LINKER_FLAGS_LINUX) -fprofile-use -o $(OBJ_NAME_PREFIX)linux64

linux64-debug : $(OBJS)
	$(CXX) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_LINUX) $(COMPILER_FLAGS_DEBUG) $(LINKER_FLAGS_LINUX) -o $(OBJ_NAME_PREFIX)linux64-debug
linux64-test : $(OBJS)
	$(CXX) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_LINUX) $(COMPILER_FLAGS_DEBUG) $(LINKER_FLAGS_LINUX) -o $(OBJ_NAME_PREFIX)linux64-test
	$(OBJ_NAME_PREFIX)linux64-test -vtl
linux64-valgrind : $(OBJS)
	$(CXX) $(OBJS) $(INCLUDE_PATHS) $(LIBRARY_PATHS) $(COMPILER_FLAGS_LINUX) $(COMPILER_FLAGS_DEBUG) $(LINKER_FLAGS_LINUX) -o $(OBJ_NAME_PREFIX)linux64-test
	valgrind --tool=memcheck --leak-check=yes $(OBJ_NAME_PREFIX)linux64-test -vtl

# default
all := linux64
test := linux64-test
.DEFAULT_GOAL := linux64
