TARGET   ?= client
CC       := gcc
CFLAG_O  := -lpthread -o 
CFLAG_CO := -c -o 
HEADERS  := -IHeaders
OBJS     := client_func.o \
            main.o \
			tcp_client.o \
			myfile.o
BIN_PATH := Sources/client_func/client_func.o \
            Sources/main/main.o \
			Sources/tcp_client/tcp_client.o \
			Sources/myfile/myfile.o
vpath %.c Sources/client_func \
          Sources/main \
		  Sources/tcp_client \
		  Sources/myfile
              
$(TARGET): $(OBJS)
	$(CC) $(BIN_PATH) $(CFLAG_O) $@

%.o:%.c
	make -C Sources/$*

.PHONY: clean
clean:
	rm $(TARGET) $(BIN_PATH) -rf

