#ifndef CLIENT_H
#define CLIENT_H

#include "asio.hpp"
#include "Message.hpp"
#include "ConnectionOwner.hpp"
#include "Connection.hpp"
#include <iostream>
#include <queue>
#include "TSQueue.hpp"

// Class representing a client that can connect to the server.
template <typename InMsgType, typename OutMsgType>
class Client {
  asio::io_context& ioContext_;
  // A client only has one connection, hence the pointer is unique.
  std::shared_ptr<Connection<InMsgType, OutMsgType>> connection_;
  
  TSQueue<OwnedMessage<InMsgType>> incomingMsgs_;

 public:
  // A client needs a context for the connection to work in, along with which endpoints it should connect to.
  Client(asio::io_context& ioContext,
         const asio::ip::tcp::resolver::results_type& endpoints)
    : ioContext_(ioContext) {
      connection_ =
      std::make_shared<Connection<InMsgType, OutMsgType>>(ioContext, incomingMsgs_, ConnectionOwner::Client);
    connection_->connectToServer(endpoints);
  }

  void connect() {
    connection_->connectToServer(ConnectionOwner::Client);
  }
  
  TSQueue<OwnedMessage<InMsgType>>& getIncomingMsgs() { return incomingMsgs_; }
  
  void send(Message<OutMsgType> msg) {
    if (connection_->isConnected()) {
      connection_->write(msg);
    }
  }

  bool isConnected() {
    return connection_->isConnected();
  }
  
  void disconnect() {
    std::cout << "Client::disconnect(): Disconnecting from server\n";
    connection_->disconnect();
  }
};

#endif
