# Путь к каталогу с заголовочными файлами SDL2 и SDL2_image
SDL_INCLUDE_DIR = src/Include

# Путь к каталогу с библиотеками SDL2 и SDL2_image
SDL_LIB_DIR = src/lib

# Компилятор и флаги
CC = g++
CFLAGS = -Wall -Wextra -I$(SDL_INCLUDE_DIR) -D_UNICODE -DUNICODE
LDFLAGS = -L$(SDL_LIB_DIR) -lSDL2 -lSDL2_image -lgdi32

# Имя исполняемого файла
TARGET = bin\basic.exe

# Объектные файлы
OBJ = main.o resource.o

# Основная цель
all: $(TARGET)

# Правило для сборки исполняемого файла
$(TARGET): $(OBJ)
	$(CC) -o $@ $^ $(LDFLAGS)
	strip $(TARGET)

# Правило для компиляции исходного файла
main.o: main.cpp
	batch_inside.bat
	$(CC) $(CFLAGS) -c -o $@ $<

resource.o: resource.rc
	windres $< -o $@

# Очистка временных файлов
clean:
	del $(OBJ) $(TARGET)