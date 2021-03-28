#include "asio.hpp"

#include <iostream>

#include "Client.hpp"
#include "GameController.hpp"
#include "GameMessage.hpp"

int main() {

  // The IO context does all the work for us.
  asio::io_context ioContext;

  // The resolver is used for resolving the host name and port.
  asio::ip::tcp::resolver resolver(ioContext);

  // Get endpoints based on host name and port.
  asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "60000");


  Client<GameMessage, PlayerAction> client(ioContext, endpoints);

  // Thread for Asio to work in.
  std::thread t([&]() { ioContext.run(); });

  GameController gameController(client);
  gameController.start();

  client.disconnect();

  // Wait for the thread that asio works in to end.
  if (t.joinable())
    t.join();
}
