#ifndef CLIENT_H
#define CLIENT_H

#include "asio.hpp"
#include "Message.hpp"

#include <iostream>
#include <queue>

template <typename T>
class Client {
  asio::io_context& ioContext_;
  asio::ip::tcp::socket socket_;

  std::queue<Message<T>> incomingMsgs_;

 public:
  Client(asio::io_context& ioContext,
         const asio::ip::tcp::resolver::results_type& endpoints)
      : ioContext_(ioContext), connection_(ioContext, incomingMsgs) {
    connection_.connectToServer(endpoints);
  }

  void connect() {
    connection_.connectToServer(ConnectionOwner::Client);
  }
  
  std::queue<Message<T>>& getIncomingMsgs() { return incomingMsgs_; }
  
  void send(Message<T> msg) {
    if (connection_.isConnected()) {
      connection.write(msg);
    }
  }

  void disconnect() {
    std::cout << "Client::disconnect(): Disconnecting from server\n";
    connection_.close();
  }
};

#endif
