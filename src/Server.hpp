#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <set>
#include <queue>

#include "asio.hpp"

#include "Connection.hpp"
#include "Message.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "TSQueue.hpp"

// Class of a single-threaded server that can be connected to multiple clients.
template <typename InMsgType, typename OutMsgType>
class Server {
  // For giving IDs to connections.
  int id_ = 0;
  
  asio::io_context& ioContext_;
  // For accepting connections.
  asio::ip::tcp::acceptor acceptor_;
  
  // The server's connections to clients.
  std::vector<std::shared_ptr<Connection<InMsgType, OutMsgType>>> connections_;
  TSQueue<OwnedMessage<InMsgType>> incomingMsgs_;
  
public:
  // Server needs a work context and which port to be reachable from.
  Server(asio::io_context& ioContext, unsigned int port)
    : ioContext_(ioContext), acceptor_(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)) {
    std::cout << "Server listening on port " << port << "\n";
    listenForConnections();
  }

  int numConnections() {
    return connections_.size();
  }

  // Get IDs of the connections. Used for syncing number of players in the game.
  std::vector<uint32_t> getIDs() {
    std::vector<uint32_t> ids;
    for (auto& connection : connections_)
      ids.push_back(connection->getID());
    return ids;
  }
  
  TSQueue<OwnedMessage<InMsgType>>& getIncomingMsgs()
  {
    return incomingMsgs_;
  }

  void disconnectFrom(std::vector<uint32_t> ids) {
    for (auto& connection : connections_) {
      if (std::find(ids.begin(), ids.end(), connection->getID()) != ids.end()) {
        connection->disconnect();
        connection.reset();
      }
    }
    connections_.erase(std::remove(connections_.begin(), connections_.end(),  nullptr), connections_.end());
  }
  
  // Write a message to all connected clients.
  void writeToAll(Message<OutMsgType> msg) {
    bool invalidClients = false;
    for (auto& connection : connections_) {
      if (connection->isConnected()) {
        connection->write(msg);
      }
      else  {
        std::cout << "Connection with ID " << connection->getID() << "is invalid\n";
        invalidClients = true;
        connection.reset(); // Release pointer's ownership of object pointed to (makes the pointer nullptr).
      }
    }

    // Remove disconnected clients, if any
    if (invalidClients) {
      connections_.erase(std::remove(connections_.begin(), connections_.end(),  nullptr), connections_.end());
      std::cout << "new num connections: " << connections_.size() << "\n";
    }
    // The call to std::remove shifts all non-null connections to the beginning and returns an iterator
    // that is one past the end of these. std::erase then deletes the elements between this iterator
    // and the end, eliminating all null connections.
  }

  // Disconnect from client with the given id.
  void disconnect(uint32_t id) {
    for (auto& connection : connections_) {
      if (connection->getID() == id) {
        connection->disconnect();
        connections_.erase(
                           std::remove_if(connections_.begin(), connections_.end(),
                                          [id](std::shared_ptr<Connection<InMsgType, OutMsgType>> c)
                                          { return c->getID() == id; }),
                           connections_.end());
        break;
      }
    }
  }
  
private:

  // Listen for clients that are trying to connect.
  void listenForConnections() {
    // The next connection to be accepted, which takes the io context as argument
    // so it can create a socket that is listened to.
    std::shared_ptr<Connection<InMsgType, OutMsgType>> connection =
      std::make_shared<Connection<InMsgType, OutMsgType>>(ioContext_, incomingMsgs_, ConnectionOwner::Server);

    // The acceptor accepts connections that connect to the given port.
    // When a connection is established via the socket, the handler is called.
    acceptor_.async_accept(connection->socket(), [this, connection](const asio::error_code& ec) {
        if (!ec) {
          std::cout << "[Client connected] " << connection->socket().remote_endpoint() << ". ID: " << id_ << "\n";
          connection->connectToClient(id_++); // Give connection an ID and start reading messages
          connections_.push_back(std::move(connection));
        }
        else
          {
            std::cout << ec.message() << "\n";
          }
        // Keep listening for connections
        listenForConnections();
      });
  }

  

};

#endif
