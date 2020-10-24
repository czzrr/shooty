#include "asio.hpp"
#include <iostream>
#include <vector>
#include <queue>

#include "player.hpp"
#include "game.hpp"
#include "message.hpp"

class client
{
public:
  client(asio::io_context& io_context, const asio::ip::tcp::resolver::results_type& endpoints,
         std::queue<game_state>& incoming_message_queue)
    : io_context_(io_context), socket_(io_context), incoming_message_queue_(incoming_message_queue)
  {
    do_connect_to_server(endpoints);
  }

  void write_to_server(player_action pa)
  {
    bool write_in_progress = !outgoing_message_queue_.empty();

    message<player_action> msg(pa);
    outgoing_message_queue_.push(msg);

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
    asio::async_read(socket_, asio::buffer(temp_in_msg.body.data(), temp_in_msg.size()),
                     [this] (const asio::error_code& ec, std::size_t /* bytes_transferred */)
                     {
                       if (!ec)
                         {
                           std::memcpy(&game_state_, temp_in_msg.body.data(), temp_in_msg.size());
                           incoming_message_queue_.push(game_state_);
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
    asio::async_write(socket_, asio::buffer(outgoing_message_queue_.front().body.data(), outgoing_message_queue_.front().size()),
                      [this] (const asio::error_code& ec, std::size_t bytes_transferred)
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
                            std::cout << "do_write_to_server: " << ec.message() << "\n";
                            do_disconnect_from_server();
                          }
                      });
  }

  asio::io_context& io_context_;
  asio::ip::tcp::socket socket_;

  game_state game_state_;
  message<game_state> temp_in_msg;

  std::queue<game_state>& incoming_message_queue_;
  std::queue<message<player_action>> outgoing_message_queue_;

};

void draw_game_state(game_state gs)
  {
    std::cout << "game_running = " << gs.game_running << ", x = " << gs.x << ", y = " << gs.y << "\n";
  }

int main() {

  asio::io_context io_context;
  asio::ip::tcp::resolver resolver(io_context);
  asio::ip::tcp::resolver::results_type endpoints = resolver.resolve("127.0.0.1", "60000");

  std::queue<game_state> incoming_msgs;
  client c(io_context, endpoints, incoming_msgs);
  std::thread t([&]() { io_context.run(); });


  std::string line;
  char action_c;
  while (std::getline(std::cin, line)) {
    while (!incoming_msgs.empty())
      {
        draw_game_state(incoming_msgs.front());
        incoming_msgs.pop();
      }
   
    for (auto ch : line)
      {
        player_action action = get_player_action(ch);
        c.write_to_server(action);
      }
  }

  // Instead of this, use strand so write operations can complete before closing socket.
  asio::steady_timer timer(io_context, asio::chrono::seconds(2));
  timer.wait();
  c.disconnect_from_server();

  // Wait for the thread that asio works in to end.
  if (t.joinable())
    t.join();
}
