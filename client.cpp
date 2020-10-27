#include "asio.hpp"
#include <iostream>
#include <vector>
#include <queue>

#include "game.hpp"
#include "player.hpp"

#include "message.hpp"

#include "SDL.h"

#include "player.hpp"
#include "bullet.hpp"
#include "game_drawer.hpp"
#include "game_controller.hpp"
#include "client.hpp"
  
int main() {

  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);
  asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "60000");

  client c(io_context, endpoints);
  std::thread t([&]() { io_context.run(); });
  
  game_controller gc(c);

  gc.start();
  
  c.disconnect_from_server();

  // Wait for the thread that asio works in to end.
  if (t.joinable())
    t.join();
}

