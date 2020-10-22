#include "asio.hpp"
#include <iostream>
#include <vector>
#include <queue>

enum class player_action : uint8_t { up, down, left, right, rotate_left, rotate_right, fire_bullet };

class player_action_message
{
public:
  player_action_message(player_action pa)
  {
    message_.push_back(pa);
  }

  player_action* data()
  {
    return message_.data();
  }
  
private:
  std::vector<player_action> message_;
};
  
class game_state
{
public:
  bool game_running;
  int x, y;
};

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

std::string get_player_action_str(player_action action)
{
  switch (action) {
  case player_action::up:
    return "up";
  case player_action::left:
    return "left";
  case player_action::down:
    return "down";
  case player_action::right:
    return "right";
  case player_action::rotate_left:
    return "rotate_left";
  case player_action::rotate_right:
    return "rotate_right";
  case player_action::fire_bullet:
    return "fire_bullet";
  }

  return "up";
}

class client
{
public:
  client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints)
    : io_context_(io_context), socket_(io_context)
  {
    do_connect_to_server(endpoints);
  }
private:
  void do_disconnect_from_server()
  {
    asio::post(io_context_, [this] () { socket_.close(); });
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
                           do_disconnect_from_server();
                         }
                     });
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

  void do_write_to_server()
  {
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().data(), write_msg_len),
                      [this] (const asio::error_code& ec, std::size_t /* bytes_transferred */)
                      {
                        if (!ec)
                          {
                            outgoing_message_queue_.pop();
                            if (!outgoing_message_queue_.empty())
                              {
                                do_write_to_server();
                              }
                          }
                        else
                          {
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

  std::queue<game_state> incoming_message_queue_;
  std::queue<player_action_message> outgoing_message_queue_;
  game_state game_state_;
};
  
int main() {

  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);
  asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "60000");

  client(io_context, endpoints);
  
  char action_c;
  while (true) {
    std::cin.get(action_c);
    if (action_c == '\n')
      break;
    player_action action = get_player_action(action_c);
    std::cout << get_player_action_str(action) << "\n";
  }
}
