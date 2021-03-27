#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "GameDrawer.hpp"
#include "Client.hpp"
#include "Game.hpp"
#include "Constants.hpp"
#include "GameMessage.hpp"

#include <map>

// Key bindings.
auto const keyUp = SDLK_w;
auto const keyDown = SDLK_s;
auto const keyLeft = SDLK_a;
auto const keyRight = SDLK_d;
auto const keyFire = SDLK_SPACE;
auto const keyRotateLeft = SDLK_LEFT;
auto const keyRotateRight = SDLK_RIGHT;

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
  GameController(Client<GameMessage, PlayerAction> & client): client_(client) {}

  // Start the controller.
  void start()
  {
    std::queue<OwnedMessage<GameMessage>>& incomingMsgs = client_.getIncomingMsgs();
    while (!quit_)
      {
        // Break out of loop if connection to server is lost.
        if (!client_.isConnected())
          {
            break;
          }
        
        while (!incomingMsgs.empty())
          {
            Game game;
            incomingMsgs.front().getData(game);
            incomingMsgs.pop();
            gameDrawer_.drawGame(game);
          }
        
        SDL_Delay(1000 / FRAMES_PER_SECOND);
        handleKeyEvents();
      }
    if (client_.isConnected())
      client_.disconnect();
    // Terminate SDL.
    gameDrawer_.close();
  }

  // Handle key input from player.
  void handleKeyEvents()
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
            auto found = keyMap_.find(e.key.keysym.sym);
            if (found != keyMap_.end())
              {
                found->second = true;
              }
          }
        else if (e.type == SDL_KEYUP)
          {
            auto found = keyMap_.find(e.key.keysym.sym);
            if (found != keyMap_.end())
              {
                found->second = false;
              }
          }

      }

    // Map down-registered keys to player actions and send them to the server.
    for (auto [keyCode, isDown] : keyMap_)
      {
        if (isDown)
          {
            Message<PlayerAction> msg;
            msg.setData(keyCodeToPlayerAction(keyCode));
            client_.send(msg);
          }
      }
  }
  
private:

  // Map of which keycodes are pressed down or not.
  std::map<SDL_Keycode, bool> keyMap_ = {{keyUp, false}, {keyDown, false}, {keyLeft, false}, {keyRight, false},
                                        {keyFire, false}, {keyRotateLeft, false}, {keyRotateRight, false}};
  bool quit_ = false;
  Client<GameMessage, PlayerAction> & client_;

  GameDrawer gameDrawer_;
  };
  
#endif
