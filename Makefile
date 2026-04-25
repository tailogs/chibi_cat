SDL_INCLUDE_DIR = src/Include

SDL_LIB_DIR = src/lib

CC = g++
CFLAGS = -Wall -Wextra -I$(SDL_INCLUDE_DIR) -D_UNICODE -DUNICODE
LDFLAGS = -L$(SDL_LIB_DIR) -lSDL2 -lSDL2_image -lgdi32

TARGET = bin\basic.exe

OBJ = main.o resource.o

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	strip $(TARGET)

main.o: main.cpp
	batch_inside.bat
	$(CC) $(CFLAGS) -c -o $@ $<

resource.o: resource.rc
	windres $< -o $@

clean:
	del $(OBJ) $(TARGET)