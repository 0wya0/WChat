TARGET ?= admin
FLAGS := -o 
CC := gcc
OBJS := main.o tcp_client.o admin.o
BIN_PATH := Sources/main/main.o \
            Sources/tcp_client/tcp_client.o \
			Sources/admin/admin.o

vpath %.c Sources/main \
          Sources/tcp_client \
		  Sources/admin

$(TARGET): $(OBJS)
	$(CC) $(BIN_PATH) $(FLAGS) $(TARGET)

%.o: %.c
	make -C Sources/$*

.PHONY: clean
clean:
	rm -rf $(BIN_PATH) $(TARGET)