#ifndef CONNECTION_TO_CLIENT_H
#define CONNECTION_TO_CLIENT_H

#include "asio.hpp"
#include <boost/archive/text_oarchive.hpp>
#include <iostream>
#include <queue>
#include "game.hpp"
#include "message.hpp"

template <typename T>
class Connection : public std::enable_shared_from_this<Connection> {
  asio::io_context& ioContext_;
  asio::ip::tcp::socket socket_;

  ConnectionOwner owner_;
  uint32_t id_;
  std::queue<OwnedMessage<T>>& incomingMsgs_;
  std::queue<Message<T>> outgoingMsgs_;

public:
  Connection(asio::io_context& ioContext,
             std::queue<OwnedMessage<T>>& incomingMsgs, ConnectionOwner owner)
    : ioContext_(ioContext),
      socket_(ioContext),
      incomingMsgs_(incomingMsgs),
      owner_(owner)
    {}

  // Should check if owner of connection is a client.
  void connectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
  {
    if (owner == ConnectionOwner::Client) {
      asio::async_connect(socket_, endpoints,
                          [this] (const asio::error_code& ec, asio::ip::tcp::endpoint /* endpoint */)
                          {
                            if (!ec)
                              {
                                readHeader();
                              }
                            else
                              {
                                std::cout << "connectToServer(): " << ec.message() << "\n";
                                disconnect_from_server();
                              }
                          });
    }
  }

  void connectToClient(uint32_t id) {
    if (owner == ConnectionOwner::Server) {
      id_ = id;
      readHeader();
    }
  }

  void listenForMessages() { readHeader(); }

  asio::ip::tcp::socket& socket() { return socket_; }

  void write(Message<T> msg) {
    auto self(shared_from_this());
    asio::post(io_context_, [this, msg, self]() {
                              bool writeInProgress = !outgoingMsgs_.empty();
                              outgoingMsgs_.push(msg);
                              if (!writeInProgress)
                                doWrite();
                            });
  }

  uint32_t getID() { return id_; }

  bool isConnected() { return socket_.is_open(); }

  // Close the connection
  void close() {
    auto self(shared_from_this());
    asio::post(io_context_, [this, self]() { socket_.close(); });
  }

private:
  void readHeader() {
    auto self(shared_from_this());
    asio::async_read(socket_,
                     asio::buffer( & tempMsg.header, tempMsg.headerSize(),
                                   [this, self](const asio::error_code & ec, std::size_t /* bytes_transferred */ ) {
                                     if (!ec) {
                                       readBody();
                                     } else {
                                       std::cout << "readHeader(): " << ec.message() << "\n";
                                       close();
                                     }
                                   });
                     }
  }

  // When the client knows how many bytes to receive, it can read the body of the message sent from the server.
  void readBody()
  {
    asio::async_read(
                     socket_, asio::buffer(tempMsg.body.data(), tempMsg.bodySize()),
                     [this](const asio::error_code& ec, std::size_t bytes_transferred) {
                       if (!ec) {
                         addToIncomingMsgs();
                         // Start another asynchronous read.
                         readHeader();
                       } else {
                         std::cout << "readBody(): " << ec.message() << "\n";
                         disconnect_from_server();
                       }
                     });
  }

  void addToIncomingMsgs(Message<T> msg) {
    if (owner == ConnectionOwner::Server) {
      incomingMsgs_.push_back(OwnedMessage<T>(this->shared_from_this(), msg));
    } else {
      incomingMsgs_.push_back(OwnedMessage<T>(nullptr, msg));
    }
  }

  void writeHeader() {
    auto self(shared_from_this());
    asio::async_write(
                      socket_, asio::buffer(&outgoingMsgs_.front().header,
                                            outgoingMsgs_.front().headerSize()),
                      [this, self](const asio::error_code& ec, std::size_t bytes_transferred) {
                        if (!ec) {
                          doSendBody();
                        } else {
                          std::cout << "do_write_to_client: " << ec.message() << "\n";
                          close();
                        }
                      });
  }

  void writeBody() {
    auto self(shared_from_this());
    asio::async_write(
                      socket_, asio::buffer(outgoingMsgs_.front().body.data(),
                                            outgoingMsgs_.front().bodySize()),
                      [this, self](const asio::error_code& ec, std::size_t bytes_transferred) {
                        if (!ec) {
                          outgoingMsgs_.pop();
                          if (!outgoingMsgs_.empty()) {
                            writeHeader();
                          }
                        } else {
                          std::cout << "do_write_to_client: " << ec.message() << "\n";
                          close();
                        }
                      });
  }
};

#endif
