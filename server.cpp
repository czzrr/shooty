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
#include <boost/serialization/vector.hpp>

#include "SDL.h"

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
        connection->write_to_client(gs);
      }
      
  }

private:
  void disconnect_from_client(std::shared_ptr<connection_to_client> conn)
  {
    connections_.erase(conn);
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

                                 connections_.insert(std::move(new_connection));

                                 
                                 new_connection->start();
                                 //std::cout << "player assigned id " << uid_ << "\n";
                                 game_.add_player(player(50, 50, uid_));
                                 uid_++;
                                 do_listen_for_client_connections();
                                 //new_connection->write_to_client(game_);
                               }
                             else
                               {
                                 std::cout << ec.message() << "\n";
                               }
                           });
  }

  
  int uid_ = 0;

  game& game_;
  
  asio::io_context& io_context_;
  
  asio::ip::tcp::acceptor acceptor_;
  
  // The server's connections to clients.
  std::set<std::shared_ptr<connection_to_client>> connections_;

  // Incoming messages from clients to server.
  std::queue<owned_player_action> incoming_message_queue_;
};

void handle_player_action(game& g, owned_player_action opa)
{
  int id = opa.get_id();
  switch (opa.get_action())
    {
    case player_action::up:
      g.move_player_up(id);
      break;
      
    case player_action::down:
      g.move_player_down(id);
      break;

    case player_action::left:
      g.move_player_left(id);
      break;

    case player_action::right:
      g.move_player_right(id);
      break;

    case player_action::fire_bullet:
      g.player_fire(id);
      break;

    case player_action::rotate_left:
      g.player_rotate_left(id);
      break;

    case player_action::rotate_right:
      g.player_rotate_right(id);
      break;
    }
}

int main()
{
  game g;
  // g.add_player(player(500, 400, 1));
  // g.add_player(player(300, 500, 2));
 
  asio::io_context io_context;
  
  unsigned int port = 60000;
  server srv(io_context, port, g);
  
  std::thread t([&]() { io_context.run(); });

  
  // asio::steady_timer timer(io_context, asio::chrono::seconds(3));
  // timer.wait();
  
  srv.write_to_clients(g);

  // asio::steady_timer timer1(io_context, asio::chrono::seconds(2));
  // timer1.wait();

  std::queue<owned_player_action>& incoming_msgs = srv.incoming();
  
  while(true)
    {
      // If any incoming messages, update game state according to them
      while (!incoming_msgs.empty())
        {
          owned_player_action opa = incoming_msgs.front();
          //std::cout << "[Player " << opa.get_id() << "]\n";
          handle_player_action(g, opa);
          incoming_msgs.pop();
        }
      
      g.advance();
      srv.write_to_clients(g);
      SDL_Delay(1000/60);
    }

  
  return 0;
}
