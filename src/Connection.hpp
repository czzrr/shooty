#ifndef CONNECTION_TO_CLIENT_H
#define CONNECTION_TO_CLIENT_H

#include "asio.hpp"
#include <iostream>
#include <queue>
#include "Game.hpp"
#include "Message.hpp"
#include "OwnedMessage.hpp"
#include "ConnectionOwner.hpp"

// Class representing a connection between two peers.
// The type of respectively incoming and outgoing messages are allowed to be different.
template <typename InMsgType, typename OutMsgType>
class Connection : public std::enable_shared_from_this<Connection<InMsgType, OutMsgType>> {
  asio::io_context& ioContext_;
  asio::ip::tcp::socket socket_;

  ConnectionOwner owner_;
  uint32_t id_;
  std::queue<OwnedMessage<InMsgType>>& incomingMsgs_;
  std::queue<Message<OutMsgType>> outgoingMsgs_;
  Message<InMsgType> tempInMsg_;

public:
  // A connection needs a context to work in, an incoming message queue and an owner.
  Connection(asio::io_context& ioContext,
             std::queue<OwnedMessage<InMsgType>>& incomingMsgs, ConnectionOwner owner)
    : ioContext_(ioContext),
      socket_(ioContext),
      incomingMsgs_(incomingMsgs),
      owner_(owner)
    {}

  // Connects a client to the server.
  void connectToServer(const asio::ip::tcp::resolver::results_type& endpoints)
  {
    if (owner_ == ConnectionOwner::Client) {
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
                                disconnect();
                              }
                          });
    }
  }

  // Connects the server to a client. The connection is given an ID.
  void connectToClient(uint32_t id) {
    if (owner_ == ConnectionOwner::Server) {
      id_ = id;
      readHeader();
    }
  }

  // Start reading messages.
  void listenForMessages() { readHeader(); }

  asio::ip::tcp::socket& socket() { return socket_; }

  // Write a message to the other peer.
  void write(Message<OutMsgType> msg) {
    auto self(this->shared_from_this());
  asio::post(ioContext_, [this, self, msg]() {
                              bool writeInProgress = !outgoingMsgs_.empty();
                              outgoingMsgs_.push(msg);
                              if (!writeInProgress)
                                writeHeader();
                            });
  }

  uint32_t getID() { return id_; }

  bool isConnected() { return socket_.is_open(); }

  // Close the connection.
  void disconnect() {
    std::cout << "Disconnecting connection with ID " << id_ << "\n";
    auto self(this->shared_from_this());
    asio::post(ioContext_, [this, self]() { socket_.close(); });
  }

private:
  // Reading a message always starts with reading a fixed-size header.
  // A temporary message object is used for convenience.
  void readHeader() {
    // shared_from_this() is used for preventing bad things if the connection pointer dies
    // after the callback handler is called but before it is finished.
    auto self(this->shared_from_this());
    asio::async_read(socket_,
                     asio::buffer(&tempInMsg_.header, sizeof(Header<InMsgType>)),
                     [this, self](const asio::error_code & ec, std::size_t /* bytes_transferred */ ) {
                                     if (!ec) {
                                       tempInMsg_.body.resize(tempInMsg_.header.size);
                                       readBody();
                                     } else {
                                       std::cout << "readHeader(): " << ec.message() << "\n";
                                       disconnect();
                                     }
                                   });
  }

  // After having read the header we know how many bytes to receive, so we can now read the body.
  void readBody() {
    auto self(this->shared_from_this());
    asio::async_read(
                     socket_, asio::buffer(tempInMsg_.body.data(), tempInMsg_.header.size),
                     [this, self](const asio::error_code& ec, std::size_t bytes_transferred) {
                       if (!ec) {
                         addToIncomingMsgs(tempInMsg_);
                         // Start another asynchronous read.
                         readHeader();
                       } else {
                         std::cout << "readBody(): " << ec.message() << "\n";
                         disconnect();
                       }
                     });
  }

  // Add a read message to the incoming message queue.
  void addToIncomingMsgs(Message<InMsgType> msg) {
    if (owner_ == ConnectionOwner::Server) {
      incomingMsgs_.push({id_, msg});
    } else {
      incomingMsgs_.push({0, msg});
    }
  }

  // Send a message, starting with the header.
  void writeHeader() {
    auto self(this->shared_from_this());
    asio::async_write(
                      socket_, asio::buffer(&outgoingMsgs_.front().header,
                                            sizeof(Header<InMsgType>)),
                      [this, self](const asio::error_code& ec, std::size_t bytes_transferred) {
                        if (!ec) {
                          writeBody();
                        } else {
                          std::cout << "writeHeader(): " << ec.message() << "\n";
                          disconnect();
                        }
                      });
  }

  // Send the message body.
  void writeBody() {
    auto self(this->shared_from_this());
    asio::async_write(
                      socket_, asio::buffer(outgoingMsgs_.front().body.data(),
                                            outgoingMsgs_.front().header.size),
                      [this, self](const asio::error_code& ec, std::size_t bytes_transferred) {
                        if (!ec) {
                          outgoingMsgs_.pop();
                          if (!outgoingMsgs_.empty()) {
                            writeHeader();
                          }
                        } else {
                          std::cout << "writeBody(): " << ec.message() << "\n";
                          disconnect();
                        }
                      });
  }
};

#endif
