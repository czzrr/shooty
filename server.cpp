#include <iostream>
#include <memory>
#include "asio.hpp"
#include <queue>
#include "player.hpp"
#include "game.hpp"
#include "message.hpp"
#include <set>

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

  void write_to_client(game_state gs)
  {
    bool write_in_progress = !outgoing_message_queue_.empty();
    outgoing_message_queue_.push(message<game_state>(gs));
    if (!write_in_progress)
      {
        do_write_to_client();
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
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().body.data(), outgoing_message_queue_.front().size()),
                      [this] (const asio::error_code& ec, std::size_t /* bytes_transferred */)
                      {
                        if (!ec)
                          {
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
  std::queue<message<game_state>> outgoing_message_queue_;
};

class server
{
public:
  server(asio::io_context& io_context, unsigned int port)
    : io_context_(io_context), acceptor_(io_context, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
  {
    std::cout << "Server listening on port " << port << "\n";
    do_listen_for_client_connections();
  }

  std::queue<owned_player_action>& incoming()
  {
    return incoming_message_queue_;
  }

  void write_to_clients(game_state gs)
  {
    bool invalid_clients = false;
    for (auto& connection : connections_)
      {
            std::cout << "writing to " << connection->socket().remote_endpoint() << "\n";
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
                                 uid_++;
                                 connections_.insert(std::move(new_connection));
                                 new_connection->start();
                                 do_listen_for_client_connections();
                               }
                             else
                               {
                                 std::cout << ec.message() << "\n";
                               }
                           });
  }

  int uid_;
  
  asio::io_context& io_context_;
  
  asio::ip::tcp::acceptor acceptor_;
  
  // The server's connections to clients.
  std::set<std::shared_ptr<connection_to_client>> connections_;

  // Incoming messages from clients to server.
  std::queue<owned_player_action> incoming_message_queue_;
};

int main()
{
  asio::io_context io_context;
  
  unsigned int port = 60000;
  server srv(io_context, port);
  
  std::thread t([&]() { io_context.run(); });

  game_state gs;
  gs.game_running = false;
  gs.x = 0;
  gs.y = 0;
  
  while(true)
    {
      // If any incoming messages, update game state according to them
      std::queue<owned_player_action>& incoming_msgs = srv.incoming();

      if (!incoming_msgs.empty())
        {
          owned_player_action opa = incoming_msgs.front();
          std::cout << "[Player " << opa.get_id() << "]: " << get_player_action_str(opa.get_action()) << "\n";
          incoming_msgs.pop();
          
          gs.game_running = true;
          gs.x++;
          gs.y++;

          srv.write_to_clients(gs);
        }
    }
  
  return 0;
}
