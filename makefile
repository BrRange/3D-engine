SRC = main.c
SLIB = -lSDL3 -lm -lkernel32 -luser32 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -luuid -ladvapi32 -lsetupapi -lshell32 -ldinput8

debug:
	gcc -o debug $(SRC) -O0 -ggdb -Iinclude -lSDL3

build:
	gcc -flto -o 3d $(SRC) -O2 -g0 -Iinclude -DNDEBUG -static $(SLIB) -mavx -msse2 -Wno-abi -fprofile-generate

release:
	gcc -flto -o 3d $(SRC) -O2 -g0 -Iinclude -DNDEBUG -static $(SLIB) -mavx -msse2 -Wno-abi -fprofile-use -mwindows