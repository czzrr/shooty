#ifndef GAME_DRAWER_H
#define GAME_DRAWER_H

#include "game.hpp"

#include "constants.hpp"

class game_drawer
{
public:
  game_drawer()
  {
    init();
  }

  bool is_init()
  {
    return is_initialized_;
  }
  
  void draw_game(game game_)
  {
    SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0x00);
    SDL_RenderClear(renderer_);

    SDL_SetRenderDrawColor(renderer_, 0xFF, 0x00, 0x00, 0xFF);
    
    for (auto player : game_.get_players())
      {
        SDL_Rect player_rect = { player.get_x(), player.get_y(), PLAYER_SIDE, PLAYER_SIDE };
        SDL_RenderFillRect(renderer_, &player_rect);

        for (auto bullet : player.get_bullets())
          {
            SDL_Rect bullet_rect = { bullet.get_x(), bullet.get_y(), BULLET_SIDE, BULLET_SIDE };
            SDL_RenderFillRect(renderer_, &bullet_rect);
          }
      }

    SDL_RenderPresent(renderer_);
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

    is_initialized_ = true;
  }

  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  bool is_initialized_ = false;
};

#endif
