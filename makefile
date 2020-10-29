all: client server

client: client.cpp
	clang++ client.cpp -I/home/cacosta/extlibs/asio-1.18.0/include/ -I/usr/include/SDL2/ -o client.out -pthread -lSDL2 -lboost_serialization -std=c++17
server: server.cpp
	clang++ server.cpp -I/home/cacosta/extlibs/asio-1.18.0/include/ -I/usr/include/SDL2/ -o server.out -pthread -lSDL2  -lboost_serialization -std=c++17


