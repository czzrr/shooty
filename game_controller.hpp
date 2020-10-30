#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "game_drawer.hpp"
#include "client.hpp"
#include "game.hpp"
#include "constants.hpp"

#include <map>

// Key bindings.
auto const key_up = SDLK_w;
auto const key_down = SDLK_s;
auto const key_left = SDLK_a;
auto const key_right = SDLK_d;
auto const key_fire = SDLK_SPACE;
auto const key_rotate_left = SDLK_LEFT;
auto const key_rotate_right = SDLK_RIGHT;

// Map key code to player action.
player_action key_code_to_player_action(SDL_Keycode key_code)
{
  switch (key_code)
    {
    case key_up:
      return player_action::up;

    case key_down:
      return player_action::down;
      
    case key_left:
      return player_action::left;
      
    case key_right:
      return player_action::right;

    case key_fire:
      return player_action::fire_bullet;

    case key_rotate_left:
      return player_action::rotate_left;

    case key_rotate_right:
      return player_action::rotate_right;
    }
}

// This class handles player input, reads incoming game states from the server and tells the game drawer to draw.
class game_controller
{
public:
  game_controller(client& c): client_(c)
  {

  }

  // Start the controller.
  void start()
  {
    std::queue<game>& incoming_games = client_.get_incoming_games();
    while (!quit_)
      {
        // Break out of loop if connection to server is lost.
        if (!client_.is_connected())
          {
            break;
          }
        
        // If the server sent game states, draw them.
        while (!incoming_games.empty())
          {
            game g = incoming_games.front();
            incoming_games.pop();
            game_drawer_.draw_game(g);
          }
        
        SDL_Delay(1000 / FRAMES_PER_SECOND);
        handle_key_events();
      }
    if (client_.is_connected())
      client_.disconnect_from_server();
    // Terminate SDL.
    game_drawer_.close();
  }

  // Handle key input from player.
  void handle_key_events()
  {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
      {
        if (e.type == SDL_QUIT)
          {
            quit_ = true;
            break;
          }
           
        else if (e.type == SDL_KEYDOWN)
          {
            // std::cout << "Pressed " << SDL_GetKeyName(e.key.keysym.sym) << " down \n";
            auto found = key_map_.find(e.key.keysym.sym);
            if (found != key_map_.end())
              {
                found->second = true;
              }
          }
        else if (e.type == SDL_KEYUP)
          {
            auto found = key_map_.find(e.key.keysym.sym);
            if (found != key_map_.end())
              {
                found->second = false;
              }
          }

      }

    // Map down-registered keys to player actions and send them to the server.
    for (auto [key_code, is_down] : key_map_)
      {
        if (is_down)
          {
            client_.write_to_server(key_code_to_player_action(key_code));
          }
      }
  }
  
private:

  // Map of which keycodes are pressed down or not.
  std::map<SDL_Keycode, bool> key_map_ = {{key_up, false}, {key_down, false}, {key_left, false}, {key_right, false},
                                          {key_fire, false}, {key_rotate_left, false}, {key_rotate_right, false}};
  bool quit_ = false;
  client& client_;

  game_drawer game_drawer_;
  };
  
#endif
