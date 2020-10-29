#ifndef CLIENT_H
#define CLIENT_H

#include "asio.hpp"
#include "SDL.h"
#include <boost/archive/text_iarchive.hpp>

#include <iostream>
#include <queue>

#include "game.hpp"
#include "message.hpp"

// This class represents the client's connection to the server.
class client
{
public:
  
  client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
    : io_context_(io_context), socket_(io_context)
  {
    // IO context and socket is initialized; connect to the server now.
    do_connect_to_server(endpoints);
  }

  // Get queue of game states sent from the server.
  std::queue<game>& get_incoming_games()
  {
    return incoming_game_queue_;
  }

  // Send a player action to the server.
  void write_to_server(player_action pa)
  {
    // WARNING: I'm not sure if accessing the outgoing queue in two different asio tasks is thread-safe.
    // Can async tasks called from the same thread run concurrently?
    asio::post(io_context_,
               [this, pa]()
               {
                 bool write_in_progress = !outgoing_message_queue_.empty();
                 message<player_action> msg(pa);
                 outgoing_message_queue_.push(msg);

                 std::cout << "write_to_server()\n";
                 std::cout << "queue size: " << outgoing_message_queue_.size() << "\n";
                 if (!write_in_progress)
                   {
                     std::cout << "write not in progress\n";
                     do_write_to_server();
                   }
                   
               });
  }

  // Disconnecting from the server means creating an asynchronous operation that closes the socket.
  void disconnect_from_server()
  {
    std::cout << "Disconnecting from server\n";
    asio::post(io_context_, [this] ()
                            {
                              socket_.close();
                            });
  }
  
private:

  // Connect to server. This is called from the constructor.
  void do_connect_to_server(const asio::ip::tcp::resolver::results_type& endpoints)
  {
    asio::async_connect(socket_, endpoints,
                        [this] (const asio::error_code& ec, asio::ip::tcp::endpoint /* endpoint */)
                        {
                          if (!ec)
                            {
                              do_read_header();
                            }
                          else
                            {
                              std::cout << "do_connect_to_server: " << ec.message() << "\n";
                              disconnect_from_server();
                            }
                        });
  }

  // Firstly, the header is read so the client knows how many bytes will be received.
  void do_read_header()
  {
    asio::async_read(socket_, asio::buffer(msg_received_.header, msg_received_.header.size()),
                     [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                     {
                       if (!ec)
                         {
                           //std::cout << "read header: " << msg_received_.header << "\n";
                           
                           // Reading header was successful. Proceed to read the body.
                           do_read_body();
                         }
                       else
                         {
                           std::cout << "do_read_header: " << ec.message() << "\n";
                           disconnect_from_server();
                         }
                     });
  }


  // When the client knows how many bytes to receive, it can read the body of the message sent from the server.
  void do_read_body()
  {
    asio::async_read(socket_, asio::buffer(msg_received_.body, msg_received_.parse_header(msg_received_.header)),
                     [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                     {
                       if (!ec)
                         {
                           //std::cout << "str_received_: " << msg_received_.body << "\nsize: " << msg_received_.body.size() << "\n";
                           // Successful read of body. Now we deserialize it.
                           std::stringstream ss;
                           ss << msg_received_.body;
                           game g;
                           {
                             boost::archive::text_iarchive ia(ss);
                             ia & g;
                           }

                           // Push the deserialized game object onto the incoming queue.
                           incoming_game_queue_.push(g);

                           // Start another asynchronous read.
                           do_read_header();
                         }
                       else
                         {
                           std::cout << "do_read_body: " << ec.message() << "\n";
                           disconnect_from_server();
                         }
                     }); 
  }

  // This function takes care of sending the player actions stored in the outgoing queue to the server.
  void do_write_to_server()
  {
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().body.data(), outgoing_message_queue_.front().size()),
                      [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                      {
                        if (!ec)
                          {
                            std::cout << "Sent to server\n";

                            // Message is sent, so pop it off the queue.
                            outgoing_message_queue_.pop();

                            // If more messages are to be send, do so.
                            if (!outgoing_message_queue_.empty())
                              {
                                do_write_to_server();
                              }
                          }
                        else
                          {
                            std::cout << "do_write_to_server: " << ec.message() << "\n";
                            disconnect_from_server();
                          }
                      });
  }

  asio::io_context& io_context_;
  asio::ip::tcp::socket socket_;

  // This is where an incoming message is temporarily stored before being converted into a game object.
  s_message msg_received_;
  
  // Queue of deserialized game objects received from the server.
  std::queue<game> incoming_game_queue_;

  // Queue of player actions that are to be send to the server.
  std::queue<message<player_action>> outgoing_message_queue_;

};

#endif
