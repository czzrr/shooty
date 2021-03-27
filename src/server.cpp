#include <iostream>
#include <thread>
#include <chrono>

#include "Server.hpp"
#include "Game.hpp"

int main()
{
  Game game;
  
  asio::io_context ioContext;
  unsigned int port = 60000;
  Server<Game> server(ioContext, port);
  std::thread t([&]() { ioContext.run(); });
  
  server.writeToAll(game);
  std::queue<OwnedMessage<Game>>& incomingMsgs = server.getIncomingMsgs();
  while(true)
    {
      // If any incoming messages, update game state according to them
      while (!incomingMsgs.empty())
        {
          OwnedMessage<Game> ownedMessage = incomingMsgs.front();
          uint32 id = ownedMessage.getID();
          Action action = messageToAction(ownedMessage.getMessage());
          if (!game.performAction(id, action))
            {
              std::cout << "player " << id << " not found\n";
              server.disconnect(id);
            }
          incomingMsgs.pop();
        }
      
      game.advance(); // Advance to next game state
      server.writeToAll(game);
      std::this_thread::sleep_for((1000 / FRAMES_PER_SECOND)ms);
    }

  
  return 0;
}
