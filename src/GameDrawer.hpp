#ifndef GAMEDRAWER_H
#define GAMEDRAWER_H

#include "SDL.h"
#include "SDL_image.h"

#include "Game.hpp"
#include "Utils.hpp"

class GameDrawer {
  SDL_Window* window_ = nullptr;
  SDL_Renderer* renderer_ = nullptr;
  bool isInitialized_ = false;
  SDL_Texture* spaceshipTexture_ = NULL;
  SDL_Texture* bulletTexture_ = NULL;

  
public:
  GameDrawer() {
    init();
    loadTextures();
  }

  void loadTextures() {
    spaceshipTexture_ = loadTexture("spaceship.png");
    bulletTexture_ = loadTexture("bullet.png");
    if(!spaceshipTexture_ || !bulletTexture_) {
      std::cout << "Failed to load textures\n";
    }
  }
  
  bool isInit() {
    return isInitialized_;
  }
  
  void drawGame(Game game_) {
    //SDL_SetRenderDrawColor(renderer_, 0x00, 0x00, 0x00, 0x00);
    SDL_SetRenderDrawColor(renderer_, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderClear(renderer_);
    
    for (auto [_, player] : game_.getPlayers()) {
      Point playerPos = player.getPos();
      SDL_Rect playerRect = { playerPos.x, playerPos.y, PLAYER_SIDE, PLAYER_SIDE };
      //SDL_RenderFillRect(renderer_, &playerRect);
      SDL_RenderCopyEx(renderer_, spaceshipTexture_, NULL, &playerRect, player.getAngle(), NULL, SDL_FLIP_NONE);
      for (auto bullet : player.getBullets()) {
        Point bulletPos = bullet.getPos();
        SDL_Rect bulletRect = { bulletPos.x, bulletPos.y, 2 * BULLET_SIDE, BULLET_SIDE};
        //SDL_RenderFillRect(renderer_, &bulletRect);
        //std::cout << bullet.getAngle() << "\n";
        SDL_RenderCopyEx(renderer_, bulletTexture_, NULL, &bulletRect, bullet.getAngle(), NULL, SDL_FLIP_NONE);
      }
    }
    SDL_RenderPresent(renderer_);
  }

  void close() {
    SDL_DestroyTexture(spaceshipTexture_);
    SDL_DestroyTexture(bulletTexture_);
    SDL_DestroyRenderer(renderer_);
    SDL_DestroyWindow(window_);
    
    IMG_Quit();
    SDL_Quit();
  }
  
private:
  void init() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
      std::cout << "SDL could not initialize. SDL_Error: " << SDL_GetError() << "\n";
      return;
    }

    if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1" )) {
      std::cout << "Warning: Linear texture filtering not enabled!\n";
      }
  
    window_ = SDL_CreateWindow("Shooty", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                               SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window_) {
      std::cout << "Window could not be created. SDL_Error: " << SDL_GetError() << "\n";
      return;
    }

  renderer_ = SDL_CreateRenderer(window_, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer_) {
      std::cout << "Renderer could not be created. SDL_Error: " << SDL_GetError() << "\n";
      return;
    }

    int imgFlags = IMG_INIT_PNG;
    if (!(IMG_Init(imgFlags) & imgFlags)) {
      std::cout << "SDL_image could not be initialized. SDL_image Error:: " << IMG_GetError() << "\n";
      return;
    }
    isInitialized_ = true;
  }

  SDL_Texture* loadTexture( std::string path ) {
    //std::cout << "loadTexture()\n";
    SDL_Texture* texture = NULL;

    SDL_Surface* surface = IMG_Load(path.c_str());
    if(!surface) {
      std::cout << "Unable to load image " << path.c_str() << ". SDL_image Error: " << IMG_GetError() << "\n";
    }
    else {
      texture = SDL_CreateTextureFromSurface(renderer_, surface);
      if(!texture) {
        std::cout << "Unable to create texture from " << path.c_str() << ". SDL Error: " << SDL_GetError() << "\n";
      }
      SDL_FreeSurface(surface);
    }
    return texture;
  }



};

#endif
