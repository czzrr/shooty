

// The server's connetion to a client.
class connection_to_client : public std::enable_shared_from_this<client_connection>
{
public:
  connection_to_client(asio::io_context& io_context, asio::ip::tcp::socket socket, int id,
                       std::queue<player_action_message>& incoming_message_queue)
    : io_context_(io_context), socket_(socket), incoming_message_queue_(incoming_message_queue), id_(id)
  {
    do_read_from_client();
  }
  
private:

  void do_read_from_client()
  {
    asio::async_read(socket_, asio::buffer(temp_player_action_msg_.data(), read_msg_len_),
                     [this] (const asio::error_code& ec, std::size_t /* bytes_transferred */)
                     {
                       if (!ec)
                         {
                           owned_player_action opa(temp_player_action_msg_.data()[0], id);
                           incoming_message_queue_.push(opa);
                           do_read_from_server();
                         }
                       else
                         {
                           do_disconnect_from_server();
                         }
                     });
  }

  void write_to_client()
  {
    bool write_in_progress = !outgoing_message_queue_.empty();
    if (!write_in_progress)
      {
        do_write_to_client();
      }
  }

  void do_write_to_client()
  {
    
  }
  

  // The clients ID. Used for controlling what to update in the game state.
  int id_;

  // The io context does all the asynchronous work for us.
  asio::io_context& io_context_;

  // The socket to 
  asio::ip::tcp::socket socket_;

  // Acts as a buffer for reading data sent from a client.
  player_action_message temp_player_action_msg_;

  // How many bytes to read from a client.
  size_t read_msg_len_ = sizeof(owned_player_move_message);

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
  std::queue<game_state> outgoing_message_queue_;
};
