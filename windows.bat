gcc -o Space.exe src/*.c -I./src/ -I./include/ -IC:/Users/k2/C++/SDL2-2.0.3/include ^
	-IC:/Users/k2/C++/glew-1.12.0-src/include -IC:/Users/k2/C++/assimp/include ^
	-LC:/Users/k2/C++/SDL2-2.0.3/lib/x86 -LC:/Users/k2/C++/glew-1.12.0-src/build ^
	-LC:/Users/k2/C++/assimp/cmake-build/code/ -Wall -g ^
	-lSDL2 -lglew32 -lglu32 -lopengl32 -lassimp -std=c99 -DSDL_MAIN_HANDLED -DGLEW_STATIC

pause