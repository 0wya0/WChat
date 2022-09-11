.PHONY: all
all: server client admin
	cp source/Server/server source/Client/client source/Admin/admin bin

server:
	make -C source/Server

client:
	make -C source/Client

admin:
	make -C source/Admin

.PHONY: clean
clean:
	make clean -C source/Server
	make clean -C source/Client
	make clean -C source/Admin
	rm bin/* -rf