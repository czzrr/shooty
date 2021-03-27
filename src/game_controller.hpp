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
PlayerAction keyCodeToPlayerAction(SDL_Keycode keyCode)
{
  switch (keyCode)
    {
    case keyUp:
      return PlayerAction::Up;

    case keyDown:
      return PlayerAction::Down;
      
    case keyLeft:
      return PlayerAction::Left;
      
    case keyRight:
      return PlayerAction::Right;

    case keyFire:
      return PlayerAction::FireBullet;

    case keyRotateLeft:
      return PlayerAction::RotateLeft;

    case keyRotateRight:
      return PlayerAction::RotateRight;
    }
}

// This class handles player input, reads incoming game states from the server and tells the game drawer to draw.
class GameController
{
public:
  GameController(Client& c): Client_(c)
  {

  }

  // Start the controller.
  void start()
  {
    std::queue<OwnedMessage<GameMessage>>& incomingMsgs = Client_.getIncomingMsgs();
    while (!quit_)
      {
        // Break out of loop if connection to server is lost.
        if (!client_.isConnected())
          {
            break;
          }
        
        while (!incomingMsgs.empty())
          {
            game g = incoming_games.front();
            incoming_games.pop();
            game_drawer_.draw_game(g);
          }
        
        SDL_Delay(1000 / FRAMES_PER_SECOND);
        handle_key_events();
      }
    if (Client_.is_connected())
      Client_.disconnect_from_server();
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
            Client_.write_to_server(key_code_to_player_action(key_code));
          }
      }
  }
  
private:

  // Map of which keycodes are pressed down or not.
  std::map<SDL_Keycode, bool> key_map_ = {{key_up, false}, {key_down, false}, {key_left, false}, {key_right, false},
                                          {key_fire, false}, {key_rotate_left, false}, {key_rotate_right, false}};
  bool quit_ = false;
  Client& Client_;

  game_drawer game_drawer_;
  };
  
#endif
