#include "asio.hpp"

#include <iostream>

#include "client.hpp"
#include "game_controller.hpp"

  
int main() {

  // The IO context does all the work for us.
  asio::io_context io_context;

  // The resolver is used for resolving the host name and port.
  asio::ip::tcp::resolver resolver(io_context);

  // Get endpoints based on host name and port.
  asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "60000");

  client c(io_context, endpoints);

  // Thread for Asio to work in.
  std::thread t([&]() { io_context.run(); });

  game_controller gc(c);
  gc.start();

  c.disconnect_from_server();

  // Wait for the thread that asio works in to end.
  if (t.joinable())
    t.join();
}

