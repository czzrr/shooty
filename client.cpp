#include "asio.hpp"
#include <iostream>
#include <vector>
#include <queue>

#include "player.hpp"
#include "game.hpp"


player_action get_player_action(char action)
{
  switch (action) {
  case 'w':
    return player_action::up;
  case 'a':
    return player_action::left;
  case 's':
    return player_action::down;
  case 'd':
    return player_action::right;
  case 'l':
    return player_action::rotate_left;
  case 'r':
    return player_action::rotate_right;
  case 'f':
    return player_action::fire_bullet;
  }

  return player_action::up;
}



class client
{
public:
  client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints, std::queue<game_state>& incoming_message_queue)
    : strand_(asio::make_strand(io_context)), io_context_(io_context), socket_(io_context), incoming_message_queue_(incoming_message_queue)
  {
    buffer_.resize(100);
    do_connect_to_server(endpoints);
  }

  void write_to_server(player_action pa)
  {
    bool write_in_progress = !outgoing_message_queue_.empty();

    player_action_message pam(pa);
    outgoing_message_queue_.push(pam);

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
    asio::async_read(socket_, asio::buffer(buffer_, read_msg_len),
                     [this] (const asio::error_code& ec, std::size_t /* bytes_transferred */)
                     {
                       if (!ec)
                         {
                           std::memcpy(&game_state_, buffer_.data(), read_msg_len);
                           incoming_message_queue_.push(game_state_);
                           //draw_game_state();
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
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().data(), write_msg_len),
                      [this] (const asio::error_code& ec, std::size_t bytes_transferred)
                      {
                        if (!ec)
                          {
                            // std::cout << "Player action "
                            //           << get_player_action_str(outgoing_message_queue_.front().msg())
                            //           << " sent to server\n";
                                                       
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
                     

  void draw_game_state()
  {
    std::cout << "x = " << game_state_.x << ", y = " << game_state_.y << "\n";
  }

  size_t read_msg_len = sizeof(game_state);
  size_t write_msg_len = 1;
  std::vector<uint8_t> buffer_;
  asio::io_context& io_context_;
  asio::ip::tcp::socket socket_;

  asio::strand<asio::io_context::executor_type> strand_;

  std::queue<game_state>& incoming_message_queue_;
  std::queue<player_action_message> outgoing_message_queue_;
  game_state game_state_;
};
  
int main() {

  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);
  asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "60000");

  std::queue<game_state> incoming_msgs;
  client c(io_context, endpoints, incoming_msgs);
  std::thread t([&]() { io_context.run(); });

  
  char action_c;
  while (true) {
    std::cin.get(action_c);
    if (action_c == '\n')
      break;
    player_action action = get_player_action(action_c);
    c.write_to_server(action);
  }

  // Instead of this, use strand so write operations can complete before closing socket.
  asio::steady_timer timer(io_context, asio::chrono::seconds(2));
  timer.wait();
  c.disconnect_from_server();

  // Wait for the thread that asio works in to end.
  if (t.joinable())
    t.join();
}
