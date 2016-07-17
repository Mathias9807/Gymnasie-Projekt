gcc -o Space.exe src/*.c -I./src/ -I./include/ -L./lib -Wall -g ^
	-lSDL2 -lglew32 -lglu32 -lopengl32 -lassimp -std=c99 -DSDL_MAIN_HANDLED -DGLEW_STATIC

pause
