CC = gcc
INC_DIR = include
OUT_DIR = build
SRC_DIR = src
TEST_DIR = tests
OUT = blake2b

CFLAGS = -Wall -I$(INC_DIR)
SRC = $(SRC_DIR)/main.c $(SRC_DIR)/blake2b.c $(SRC_DIR)/utils.c
OBJ = $(SRC:$(SRC_DIR)/%.c=build/%.o)
TARGET = $(OUT_DIR)/$(OUT)

all: $(TARGET)

$(TARGET): $(OBJ)
	mkdir -p $(OUT_DIR)
	$(CC) -o $@ $^

build/%.o: $(SRC_DIR)/%.c
	mkdir -p build
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf $(OUT_DIR)/*.o $(TARGET)

test: $(OBJ) $(TEST_DIR)/test_blake2b.c
	mkdir -p build
	$(CC) $(CFLAGS) -o $(OUT_DIR)/test_blake2b $(TEST_DIR)/test_blake2b.c $(SRC_DIR)/blake2b.c $(SRC_DIR)/utils.c
	./$(OUT_DIR)/test_blake2b