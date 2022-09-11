TARGET   ?= server
CC       := gcc
CFLAG_O  := -lpthread -lsqlite3 -o 
CFLAG_CO := -c -o 
BIN_PATH := Sources/cpp/
HEADERS  := -IHeaders
OBJS     := main.o \
			myepoll.o \
			tcp_server.o \
			tpool.o \
			server_func.o \
			mysqlite.o \
			online.o \
			myfile.o

BINS     := $(BIN_PATH)main/main.o \
            $(BIN_PATH)myepoll/myepoll.o \
			$(BIN_PATH)tcp_server/tcp_server.o \
			$(BIN_PATH)tpool/tpool.o \
			$(BIN_PATH)/server_func/server_func.o \
			$(BIN_PATH)mysqlite/mysqlite.o \
			$(BIN_PATH)online/online.o \
			$(BIN_PATH)myfile/myfile.o

vpath %.c $(BIN_PATH)main \
          $(BIN_PATH)myepoll \
		  $(BIN_PATH)tcp_server \
		  $(BIN_PATH)tpool \
		  $(BIN_PATH)server_func \
		  $(BIN_PATH)mysqlite \
		  $(BIN_PATH)online \
		  $(BIN_PATH)myfile
              
$(TARGET): $(OBJS)
	$(CC) $(BINS) $(CFLAG_O) $@

%.o:%.c
	make -C $(BIN_PATH)$*

.PHONY: clean
clean:
	rm $(TARGET) $(BINS) -rf