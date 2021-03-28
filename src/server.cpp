#include <iostream>
#include <thread>
#include <chrono>

#include "Server.hpp"
#include "Game.hpp"
#include "GameMessage.hpp"

int main()
{
  Game game;
  
  asio::io_context ioContext;
  unsigned int port = 60000;
  Server<PlayerAction, GameMessage> server(ioContext, port);
  std::thread t([&]() { ioContext.run(); });
  
  //server.writeToAll(game);
  std::queue<OwnedMessage<PlayerAction>>& incomingMsgs = server.getIncomingMsgs();
  int numPlayers = 0;
  while(true)
    {
      if (game.getNumPlayers() != server.numConnections()) {
        game.syncPlayers(server.getIDs());
      }
      // If any incoming messages, update game state according to them
      while (!incomingMsgs.empty())
        {
          OwnedMessage<PlayerAction> ownedMessage = incomingMsgs.front();
          uint32_t id = ownedMessage.id;
          PlayerAction action = ownedMessage.msg.header.messageId;
          if (!game.performAction(id, action))
            {
              std::cout << "player " << id << " not found\n";
              server.disconnect(id);
            }
          incomingMsgs.pop();
        }


  
      game.advance(); // Advance to next game state
      Message<GameMessage> msg;
      msg.setMessageId(GameMessage::GameState);
      msg.setData(game);
      server.writeToAll(msg);
      std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FRAMES_PER_SECOND));
    }

  
  return 0;
}
