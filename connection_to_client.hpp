#ifndef CONNECTION_TO_CLIENT_H
#define CONNECTION_TO_CLIENT_H

#include "asio.hpp"
#include <boost/archive/text_oarchive.hpp>

#include <iostream>
#include <queue>

#include "game.hpp"
#include "message.hpp"

const int BYTES_TO_RECEIVE = 1; // At the moment it is always the case that the number of bytes to be received is 1.

// TODO: the classes 'client' and connection_to_client' are similar enough that we can get by with
// only using one 'connection' class if we do some generalizing.

// The server's connection to a client.
class connection_to_client : public std::enable_shared_from_this<connection_to_client>
{
public:
  connection_to_client(asio::io_context& io_context, int id,
                       std::queue<owned_player_action>& incoming_message_queue, std::set<std::shared_ptr<connection_to_client>>& connections)
    : io_context_(io_context), socket_(io_context), incoming_message_queue_(incoming_message_queue), id_(id), connections_(connections)
  {
    msg_received_.body.resize(BYTES_TO_RECEIVE); // Make BYTES_TO_RECEIVE bytes available to be received.
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
    asio::async_read(socket_, asio::buffer(msg_received_.body, BYTES_TO_RECEIVE),
                     [this, self] (const asio::error_code& ec, std::size_t /* bytes_transferred */)
                     {
                       if (!ec)
                         {
                           player_action pa = static_cast<player_action>(std::stoi(msg_received_.body));
                           owned_player_action opa(pa, id_);
                           incoming_message_queue_.push(opa);

                           do_read_from_client();
                         }
                       else
                         {
                           std::cout << "do_read_from_client: " << ec.message() << "\n";
                           // remove_client();
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
  
  // The client's ID. Used for controlling what to update in the game state.
  int id_;

  // The io context does all the asynchronous work for us.
  asio::io_context& io_context_;

  // The socket to 
  asio::ip::tcp::socket socket_;

  std::set<std::shared_ptr<connection_to_client>>& connections_;
  
  // Acts as a buffer for reading data sent from a client.
  s_message msg_received_;

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

#endif
