//
// chat_client.cpp
// ~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2020 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <deque>
#include <iostream>
#include <thread>
#include "asio.hpp"
#include "chat_message.hpp"

using asio::ip::tcp;

typedef std::deque<player_move> player_move_queue;

class client
{
public:
  client(asio::io_context& io_context,
      const tcp::resolver::results_type& endpoints)
    : io_context_(io_context),
      socket_(io_context)
  {
    do_connect(endpoints);
  }

  void write(const player_move& msg)
  {
    asio::post(io_context_,
        [this, msg]()
        {
          bool write_in_progress = !write_msgs_.empty();
          write_msgs_.push_back(msg);
          if (!write_in_progress)
          {
            do_write();
          }
        });
  }

  void close()
  {
    // Add the task of closing to socket to the asio context's task queue.
    asio::post(io_context_, [this]() { socket_.close(); });
  }

private:
  void do_connect(const tcp::resolver::results_type& endpoints)
  {
    // Create an asynchronous connect task.
    // Connect the client to the server through the socket when it [the socket] is available.
    asio::async_connect(socket_, endpoints,
        [this](std::error_code ec, tcp::endpoint)
        {
          // When there are no errors, an asynchronous task for reading messages from the server is created.
          if (!ec)
          {
            do_read_header();
          }
        });
  }

  void do_read_header()
  {
    // Create an asynchronous read task.
    // When data is available for reading through the socket,
    // read the header part and store it in the read_msg_ header part.
    // Proceed to call do_read_body if there are no errors and the header is valid.
    asio::async_read(socket_,
        asio::buffer(read_msg_.data(), player_move::header_length),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec && read_msg_.decode_header())
          {
            do_read_body();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_read_body()
  {
    // Create an asynchronous read task, meaning data will be read when it is available.
    // When data is available for reading through the socket,
    // read the body part and store it in the read_msg_ body part.
    // If there are no errors, write the message to the console
    // then call do_read_header to set up a new asynchronous task for reading a new message.
    asio::async_read(socket_,
        asio::buffer(read_msg_.body(), read_msg_.body_length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            std::cout.write(read_msg_.body(), read_msg_.body_length());
            std::cout << "\n";
            do_read_header();
          }
          else
          {
            socket_.close();
          }
        });
  }

  void do_write()
  {
    // Create an asynchronous write task.
    // Data will be written when the socket is available.
    asio::async_write(socket_,
        asio::buffer(write_msgs_.front().data(),
          write_msgs_.front().length()),
        [this](std::error_code ec, std::size_t /*length*/)
        {
          if (!ec)
          {
            // Remove the sent message from the queue.
            write_msgs_.pop_front();
            
            // If there are more messages to be sent, create a new asynchronous write task.
            if (!write_msgs_.empty())
            {
              do_write();
            }
          }
          else
          {
            socket_.close();
          }
        });
  }

private:
  asio::io_context& io_context_;
  tcp::socket socket_;
  player_move read_msg_;
  player_move_queue write_msgs_;
};

int main(int argc, char* argv[])
{
  try
  {
    // if (argc != 3)
    // {
    //   std::cerr << "Usage: client <host> <port>\n";
    //   return 1;
    // }

    // The IO context does all the asynchronous tasks.
    asio::io_context io_context;

    // The resolver creates endpoints from the given host address and port number.
    tcp::resolver resolver(io_context);
    auto endpoints = resolver.resolve("127.0.0.1", "60000");

    // The chat client needs the IO context for priming it with asynchronous tasks and the endpoints to connect to.
    client c(io_context, endpoints);

    // We create a thread for the IO context to run in.
    std::thread t([&io_context](){ io_context.run(); });

    char line[player_move::max_body_length + 1];
    while (std::cin.getline(line, player_move::max_body_length + 1))
    {
      player_move msg;
      msg.body_length(std::strlen(line)); // Set message length to length of entered string.
      std::memcpy(msg.body(), line, msg.body_length()); // Copy entered string into message body.
      msg.encode_header(); // Encode length of message body into header.
      c.write(msg); // Send the message to the server.
    }
    // The user interrupted the loop.
    //Close the client's connection and close the IO context's thread.
    c.close();
    t.join();
  }
  catch (std::exception& e)
  {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
