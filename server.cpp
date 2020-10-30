#include <iostream>
#include <memory>
#include "asio.hpp"
#include <queue>
#include "player.hpp"
#include "game.hpp"
#include "message.hpp"
#include <set>


#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "SDL.h"

#include "constants.hpp"
#include "connection_to_client.hpp"

class server
{
public:
  server(asio::io_context& io_context, unsigned int port, game& g)
    : io_context_(io_context), acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), game_(g)
  {
    std::cout << "Server listening on port " << port << "\n";
    do_listen_for_client_connections();
  }

  std::queue<owned_player_action>& incoming()
  {
    return incoming_message_queue_;
  }

  void write_to_clients(game gs)
  {
    bool invalid_clients = false;
    for (auto& connection : connections_)
      {
        if (connection->is_connected())
          {
            connection->write_to_client(gs);
          }
        else
          {
            invalid_clients = true;
            connection.reset();
          }
      }

    if (invalid_clients)
      connections_.erase(std::remove(connections_.begin(), connections_.end(),  nullptr), connections_.end());
  }
  
  void disconnect_from_client(int id)
  {
    for (auto conn : connections_)
      {
        if (conn->id() == id)
          {
            disconnect_from_client(conn);
          }
      }
  }
  
private:
  void disconnect_from_client(std::shared_ptr<connection_to_client> conn)
  {
    game_.remove_player(conn->id());
    connections_.erase(std::remove(connections_.begin(), connections_.end(), conn), connections_.end());
  }

 
  
  void do_listen_for_client_connections()
  {
    std::shared_ptr<connection_to_client> new_connection =
      std::make_shared<connection_to_client>(io_context_, uid_, incoming_message_queue_, connections_);

    acceptor_.async_accept(new_connection->socket(), [this, new_connection](const asio::error_code& ec)
                           {
                             if (!ec)
                               {
                                 std::cout << "[Client connected] " << new_connection->socket().remote_endpoint() << "\n";
                                 connections_.push_back(std::move(new_connection));
                                 new_connection->start();
                                 //std::cout << "player assigned id " << uid_ << "\n";
                                 game_.add_player(uid_++);
                               }
                             else
                               {
                                 std::cout << ec.message() << "\n";
                               }
                             do_listen_for_client_connections();
                           });
  }

  
  int uid_ = 0;

  game& game_;
  
  asio::io_context& io_context_;

  // For accepting connections.
  asio::ip::tcp::acceptor acceptor_;
  
  // The server's connections to clients.
  std::vector<std::shared_ptr<connection_to_client>> connections_;

  // Incoming messages from clients to server.
  std::queue<owned_player_action> incoming_message_queue_;
};

int main()
{
  game g;
  
  asio::io_context io_context;
  unsigned int port = 60000;
  server srv(io_context, port, g);
  std::thread t([&]() { io_context.run(); });
  
  srv.write_to_clients(g);
  std::queue<owned_player_action>& incoming_msgs = srv.incoming();
  while(true)
    {
      // If any incoming messages, update game state according to them
      while (!incoming_msgs.empty())
        {
          owned_player_action opa = incoming_msgs.front();
          //std::cout << opa.get_id() << ":" << get_player_action_str(opa.get_action()) << "\n";
          if (!g.do_action(opa.get_id(), opa.get_action()))
            {
              srv.disconnect_from_client(opa.get_id());
            }
          incoming_msgs.pop();
        }
      
      g.advance();
      srv.write_to_clients(g);
      SDL_Delay(1000 / FRAMES_PER_SECOND);
    }

  
  return 0;
}
