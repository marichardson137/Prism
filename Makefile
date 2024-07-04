OSX_COMPILER = clang
WIN_COMPILER = g++
SOURCE_LIBS = -Ilib/

# MacOS specific flags and libraries
OSX_OPT =  -g -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib_osx.a lib/libseidel_osx.a -std=c++17
OSX_OUT = -o "bin/build_osx"
CFILES = src/*.cpp

build_osx:
	$(OSX_COMPILER) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)

# Windows specific flags and libraries
WIN_OPT = -g -Llib/ lib/libraylib_win.a lib/libseidel_win.a -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++ -std=c++17
WIN_OUT = -o "bin/build_win.exe"
CFILES = src/*.cpp

build_win:
	$(WIN_COMPILER) $(CFILES) $(SOURCE_LIBS) $(WIN_OUT) $(WIN_OPT)

run:
	make build_osx
	./bin/build_osx
	