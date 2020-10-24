all: client server

client: client.cpp
	clang++ client.cpp -I/home/cacosta/extlibs/asio-1.18.0/include/ -o client.out -pthread
server: server.cpp
	clang++ server.cpp -I/home/cacosta/extlibs/asio-1.18.0/include/ -o server.out -pthread


