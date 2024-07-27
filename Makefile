OSX_COMPILER = clang++
WIN_COMPILER = g++
SOURCE_LIBS = -Ilib/
CFILES = src/*.cpp

# MacOS specific flags and libraries
OSX_OPT =  -g -Llib/ -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -framework OpenGL lib/libraylib_osx.a lib/libseidel_osx.a -std=c++17
OSX_OUT = -o "bin/build_osx"

build_osx:
	$(OSX_COMPILER) $(CFILES) $(SOURCE_LIBS) $(OSX_OUT) $(OSX_OPT)

# Windows specific flags and libraries
WIN_OPT = -g -Llib/ lib/libraylib_win.a lib/libseidel_win.a -lopengl32 -lgdi32 -lwinmm -static-libgcc -static-libstdc++ -std=c++17
WIN_OUT = -o "bin/build_win.exe"

build_win:
	$(WIN_COMPILER) $(CFILES) $(SOURCE_LIBS) $(WIN_OUT) $(WIN_OPT)

build_web:
	emcc -o prism.html $(CFILES) -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces -Wunused-result -Os $(SOURCE_LIBS) -Llib/ lib/libraylib_web.a -s USE_GLFW=3 -s ASYNCIFY -s TOTAL_MEMORY=67108864 -s FORCE_FILESYSTEM=1 --shell-file src/shell.html -DPLATFORM_WEB -s 'EXPORTED_FUNCTIONS=["_free","_malloc","_main"]' -s EXPORTED_RUNTIME_METHODS=ccall

run:
	make build_osx
	./bin/build_osx
	