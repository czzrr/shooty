#ifndef GAMEDRAWER_H
#define GAMEDRAWER_H

#include "Game.hpp"
#include "Constants.hpp"

class GameDrawer {
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  bool isInitialized_ = false;
  
public:
  GameDrawer()
  {
    init();
  }

  bool isInit()
  {
    return isInitialized_;
  }
  
  void drawGame(Game game_)
  {
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer_);

    SDL_SetRenderDrawColor(renderer_, 0xFF, 0x00, 0x00, 0xFF);
    
    for (auto [_, player] : game_.getPlayers())
      {
        Point playerPos = player.getPos();
        SDL_Rect playerRect = { playerPos.getX(), playerPos.getY(), PLAYER_SIDE, PLAYER_SIDE };
        SDL_RenderFillRect(renderer_, &playerRect);

        for (auto bullet : player.getBullets())
          {
            Point bulletPos = bullet.getPos();
            SDL_Rect bulletRect = { bulletPos.getX(), bulletPos.getY(), BULLET_SIDE, BULLET_SIDE };
            SDL_RenderFillRect(renderer_, &bulletRect);
          }
      }

    SDL_RenderPresent(renderer_);
  }

  void close()
  {
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    SDL_Quit();
  }
  
private:

  void init()
  {
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
      {
        std::cout << "SDL could not initialize. SDL_Error: " << SDL_GetError() << "\n";
        return;
      }
  
    window_ = SDL_CreateWindow("Shooty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window_)
      {
        std::cout << "Window could not be created. SDL_Error: " << SDL_GetError() << "\n";
        return;
      }

    renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_)
      {
        std::cout << "Renderer could not be created. SDL_Error: " << SDL_GetError() << "\n";
        return;
      }

    isInitialized_ = true;
  }

};

#endif
