#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <set>
#include <queue>

#include "asio.hpp"

#include "Connection.hpp"
#include "Message.hpp"
#include "player.hpp"
#include "game.hpp"

// A single-threaded server
template <typename T>
class Server {
  int uid_ = 0;
  
  asio::io_context& ioContext_;
  // For accepting connections.
  asio::ip::tcp::acceptor acceptor_;
  
  // The server's connections to clients.
  std::vector<std::shared_ptr<Connection<T>>> connections_;
  std::queue<OwnedMessage<T>> incomingMsgs_;
  
public:
  server(asio::io_context& ioContext, unsigned int port)
    : ioContext_(ioContext), acceptor_(ioContext, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port))
  {
    std::cout << "Server listening on port " << port << "\n";
    listenForConnections();
  }

  std::queue<Message<T>>& getIncomingMsgs()
  {
    return incomingMsgs_;
  }

  void writeToAll(Message<T> msg)
  {
    bool invalidClients = false;
    for (auto& connection : connections_)
      {
        if (connection->isConnected())
          {
            connection->write(msg);
          }
        else 
          {
            invalidClients = true;
            connection.reset(); // Release pointer's ownership of object pointed to (makes the pointer nullptr).
          }
      }

    // Remove disconnected clients, if any
    if (invalidClients)
      connections_.erase(std::remove(connections_.begin(), connections_.end(),  nullptr), connections_.end());
    // The call to std::remove shifts all non-null connections to the beginning and returns an iterator
    // that is one past the end of these. std::erase then deletes the elements between this iterator
    // and the end, eliminating all null connections.
  }

  // Remove player from game and connection by id.
  // Happens when some async task failed or player died.
  void disconnect(int id)
  {
    for (auto& connection : connections_)
      {
        if (connection->getID() == id)
          {
            connection->close();
            connections_.erase(
                               std::remove_if(connections_.begin(), connections_.end(),
                                              [id](std::shared_ptr<Connection<T>> c)
                                              { return c->getID() == id; }),
                               connections_.end());
            break;
          }
      }

    

  }
  
private:
 
  void listenForConnections()
  {
    // The next connection that is accepted, which takes the io context as argument so it can create a socket
    // that is listened to.
    std::shared_ptr<connection<T>> connection =
      std::make_shared<connection<T>>(ioContext, incomingMsgs_, connections_, ConnectionOwner::Server);

    // The acceptor accepts connections that connect to the given port.
    // When a connection is established via the socket, the handler is called.
    acceptor_.async_accept(connection->socket(), [this, connection](const asio::error_code& ec)
                           {
                             if (!ec)
                               {
                                 std::cout << "[Client connected] " << connection->socket().remote_endpoint() << "\n";
                                 connection_.connectToClient(id_++); // Give connection an ID and start reading messages
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
