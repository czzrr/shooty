#ifndef CLIENT_H
#define CLIENT_H

#include "asio.hpp"
#include <iostream>
#include <vector>
#include <queue>

#include "player.hpp"
#include "game.hpp"
#include "message.hpp"

#include "SDL.h"

#include "player.hpp"
#include "bullet.hpp"
#include "game_drawer.hpp"
#include "game_controller.hpp"

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/serialization/vector.hpp>

class client
{
public:
  client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
    : io_context_(io_context), socket_(io_context)
  {
    game_.add_player(player(100, 100, 1));
    game_.add_player(player(300, 500, 2));
    do_connect_to_server(endpoints);
  }

  std::queue<game>& get_incoming_msgs()
  {
    return incoming_message_queue_;
  }
  
  void write_to_server(player_action pa)
  {
    bool write_in_progress = !outgoing_message_queue_.empty();

    message<player_action> msg(pa);
    outgoing_message_queue_.push(msg);

    // Only start an asynchronous write task if one is not already in progress.
    if (!write_in_progress)
      {
        do_write_to_server();
      }
  }

  void disconnect_from_server()
  {
    do_disconnect_from_server();
  }
  
private:
  void do_disconnect_from_server()
  {
    std::cout << "Disconnecting from server\n";
    asio::post(io_context_, [this] ()
                            {
                              socket_.close();
                            });
  }
  
  void do_connect_to_server(const asio::ip::tcp::resolver::results_type& endpoints)
  {
    asio::async_connect(socket_, endpoints,
                        [this] (const asio::error_code& ec, asio::ip::tcp::endpoint /* endpoint */)
                        {
                          if (!ec)
                            {
                              do_read_from_server();
                            }
                          else
                            {
                              std::cout << "do_connect_to_server: " << ec.message() << "\n";
                              do_disconnect_from_server();
                            }
                        });
  }

  void do_read_from_server()
  {
    do_read_header();
  }

  void do_read_header()
  {
    asio::async_read(socket_, asio::buffer(msg_received_.header, msg_received_.header.size()),
                     [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                     {
                       if (!ec)
                         {
                           //std::cout << "read header: " << msg_received_.header << "\n";
                           do_read_body();
                         }
                       else
                         {
                           std::cout << "do_read_from_server: " << ec.message() << "\n";
                           do_disconnect_from_server();
                         }
                     });
  }


  void do_read_body()
  {
    asio::async_read(socket_, asio::buffer(msg_received_.body, msg_received_.parse_header(msg_received_.header)),
                     [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                     {
                       if (!ec)
                         {
                           //std::cout << "str_received_: " << msg_received_.body << "\nsize: " << msg_received_.body.size() << "\n";
                           std::stringstream ss;
                           ss << msg_received_.body;
                           {
                             boost::archive::text_iarchive ia(ss);
                             ia & game_;
                           }

                           incoming_message_queue_.push(game_);
                           do_read_from_server();
                         }
                       else
                         {
                           std::cout << "do_read_from_server: " << ec.message() << "\n";
                           do_disconnect_from_server();
                         }
                     }); 
  }
  
  void do_write_to_server()
  {
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().body.data(), outgoing_message_queue_.front().size()),
                      [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                      {
                        if (!ec)
                          {
                            std::cout << "Sent to server\n";
                            outgoing_message_queue_.pop();
                            if (!outgoing_message_queue_.empty())
                              {
                                do_write_to_server();
                              }
                          }
                        else
                          {
                            std::cout << "do_write_to_server: " << ec.message() << "\n";
                            do_disconnect_from_server();
                          }
                      });
  }

  s_message msg_received_;
  
  asio::io_context& io_context_;
  asio::ip::tcp::socket socket_;

  game game_;
  message<game> temp_in_msg;

  std::queue<game> incoming_message_queue_;
  std::queue<message<player_action>> outgoing_message_queue_;

};

#endif
