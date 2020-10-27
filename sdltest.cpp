//Using SDL and standard IO
#include "SDL.h"
#include <iostream>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

int main()
{
  SDL_Window* window = nullptr;

  SDL_Surface* screen_surface = nullptr;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
      std::cout << "SDL could not initialize. SDL_Error: " << SDL_GetError() << "\n";
      return -1;
    }
  
  window = SDL_CreateWindow("My Window", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                           SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (!window)
    {
      std::cout << "Window could not be created. SDL_Error: " << SDL_GetError() << "\n";
      return -1;
    }

  screen_surface = SDL_GetWindowSurface(window);

  SDL_FillRect(screen_surface, nullptr, SDL_MapRGB(screen_surface->format, 0xFF, 0xFF, 0xFF));

  SDL_UpdateWindowSurface(window);

  SDL_Event e;
  bool quit = false;
  while (!quit)
    {
      while (SDL_PollEvent(&e) != 0)
        {
          if (e.type == SDL_QUIT)
            {
            quit = true;
            }
          else if (e.type == SDL_KEYDOWN)
            {
              switch(e.key.keysym.sym)
                {
                case SDLK_UP:
                  std::cout << "Up\n";
                  break;
                  
                case SDLK_DOWN:
                  std::cout << "Down\n";
                  break;
                  
                case SDLK_LEFT:
                  std::cout << "Left\n";
                  break;

                case SDLK_RIGHT:
                  std::cout << "Right\n";
                  break;
            }
              
            }
        }
    }


  SDL_DestroyWindow(window);

  SDL_Quit();

  return 0;
}
