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

// The server's connection to a client.
class connection_to_client : public std::enable_shared_from_this<connection_to_client>
{
public:
  connection_to_client(asio::io_context& io_context, int id,
                       std::queue<owned_player_action>& incoming_message_queue, std::set<std::shared_ptr<connection_to_client>>& connections)
    : io_context_(io_context), socket_(io_context), incoming_message_queue_(incoming_message_queue), id_(id), connections_(connections)
  {
  }

  void start()
  {
    do_read_from_client();
  }
  
  asio::ip::tcp::socket& socket()
  {
    return socket_;
  }

  void write_to_client(game g)
  {
    std::stringstream ss;
    {
      boost::archive::text_oarchive oa(ss);
      oa & g;
    }

    std::string str_to_send = ss.str();
    
    //std::cout << "str_to_send: " << str_to_send << "\nsize: " << str_to_send.size() << "\n";

    s_message msg;
    if (msg.set_header(str_to_send.size()))
      {
        bool write_in_progress = !outgoing_message_queue_.empty();
        msg.body = str_to_send;
        outgoing_message_queue_.push(msg);
        if (!write_in_progress)
          {
            do_write_to_client();
          }
      }

  }
  
private:
  
  void do_read_from_client()
  {
    auto self(shared_from_this());
    asio::async_read(socket_, asio::buffer(temp_in_msg_.body.data(), temp_in_msg_.size()),
                     [this, self] (const asio::error_code& ec, std::size_t /* bytes_transferred */)
                     {
                       if (!ec)
                         {
                           player_action pa;
                           std::memcpy(&pa, temp_in_msg_.body.data(), temp_in_msg_.size());
                           owned_player_action opa(pa, id_);
                           incoming_message_queue_.push(opa);
                           do_read_from_client();
                         }
                       else
                         {
                           std::cout << "do_read_from_client: " << ec.message() << "\n";
                           //do_read_from_client();
                           //do_disconnect_from_client();
                           connections_.erase(shared_from_this());
                         }
                     });
  }

  void do_write_to_client()
  {
    do_send_header();
    
  }

  void do_send_header()
  {
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().header, outgoing_message_queue_.front().header.size()),
                      [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                      {
                        if (!ec)
                          {
                            //std::cout << "Sent " << bytes_transferred << " bytes to client.\n";

                            do_send_body();
                          }
                        else
                          {
                            std::cout << "do_write_to_client: " << ec.message() << "\n";
                            //do_disconnect_from_client();
                          }
                      });
  }

  void do_send_body()
  {
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().body, outgoing_message_queue_.front().header_n),
                      [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                      {
                        if (!ec)
                          {
                            //std::cout << "Sent " << bytes_transferred << " bytes to client.\n";
                            outgoing_message_queue_.pop();
                            if (!outgoing_message_queue_.empty())
                              {
                                do_write_to_client();
                              }
                          }
                        else
                          {
                            std::cout << "do_write_to_client: " << ec.message() << "\n";
                            //do_disconnect_from_client();
                          }
                      });
  }
  
  // The clients ID. Used for controlling what to update in the game state.
  int id_;

  // The io context does all the asynchronous work for us.
  asio::io_context& io_context_;

  // The socket to 
  asio::ip::tcp::socket socket_;

  std::set<std::shared_ptr<connection_to_client>>& connections_;
  
  // Acts as a buffer for reading data sent from a client.
  message<player_action> temp_in_msg_;

  // Nevermind this; since we only run io context from one thread, callback handlers cannot run concurrently.
  // ---
  // The server's incoming message queue must be thread safe,
  // since there can be multiple clients connected to the server simultaneously
  // (and asio may use threads internally).
  
  // Another option would be to have an unique incoming message queue for each connection.
  // Then the server would need to need to check each of these queues for incoming messages.
  // That could become costly with many clients, though.

  // This is a reference as it's provided by the server.
  std::queue<owned_player_action>& incoming_message_queue_;

  // The queue where the server puts the messages that are to be sent to the connected client.
  std::queue<s_message> outgoing_message_queue_;
};

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
        //std::cout << "writing to " << connection->socket().remote_endpoint() << "\n";
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

  
  asio::steady_timer timer(io_context, asio::chrono::seconds(3));
  timer.wait();
  
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
