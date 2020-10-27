#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "game_drawer.hpp"
#include "client.hpp"
#include "game.hpp"

auto const key_up = SDLK_w;
auto const key_down = SDLK_s;
auto const key_left = SDLK_a;
auto const key_right = SDLK_d;
auto const key_fire = SDLK_SPACE;
auto const key_rotate_left = SDLK_LEFT;
auto const key_rotate_right = SDLK_RIGHT;

class game_controller
{
public:
  game_controller(client& c): client_(c)
  {
    
  }

  void start()
  {
    while (!quit_)
      {
        std::queue<game>& incoming_msgs = client_.get_incoming_msgs();
        while (!incoming_msgs.empty())
          {
            std::cout << "accessing game object from server\n";
            game g = incoming_msgs.front();
            std::cout << "accessed game object from server\n";
            incoming_msgs.pop();
            g.get_players();
            std::cout << "trying to draw game\n";
            game_drawer_.draw_game(g);
            
            //game_.advance();
          }
        SDL_Delay(1000 / FRAMES_PER_SECOND);
        handle_key_events();
        
      }
  }

  void handle_key_events()
  {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0)
      {
        if (e.type == SDL_QUIT)
          {
            quit_ = true;
          }
           
        else if (e.type == SDL_KEYDOWN)
          {
            switch(e.key.keysym.sym)
              {
              case key_up:
                std::cout << "Up\n";
                u_ = true;
                break;
                  
              case key_down:
                std::cout << "Down\n";
                d_ = true;
                break;
                  
              case key_left:
                std::cout << "Left\n";
                l_ = true;
                break;

              case key_right:
                std::cout << "Right\n";
                r_ = true;
                break;

              case key_fire:
                std::cout << "Fire\n";
                f_ = true;
                break;

              case key_rotate_right:
                std::cout << "Rotate right\n";
                rr_ = true;
                break;

              case key_rotate_left:
                std::cout << "Rotate left\n";
                rl_ = true;
                break;
              }
          }
        else if (e.type == SDL_KEYUP)
          {
            switch(e.key.keysym.sym)
              {
              case key_up:
                u_ = false;
                break;
                  
              case key_down:
                d_ = false;
                break;
                  
              case key_left:
                l_ = false;
                break;

              case key_right:
                r_ = false;
                break;

              case key_fire:
                f_ = false;
                break;

              case key_rotate_right:
                rr_ = false;
                break;
              case key_rotate_left:
                rl_ = false;
                break;
              }
          }
      }
    if (u_)
      client_.write_to_server(player_action::up);
    // if (d_)
    //   game_.move_player_down(1);
    // if (l_)
    //   game_.move_player_left(1);
    // if (r_)
    //   game_.move_player_right(1);
    // if (f_)
    //   game_.player_fire(1);
    // if (rr_)
    //   game_.player_rotate_right(1);
    // if (rl_)
    //   game_.player_rotate_left(1);
  }
  
private:
  bool quit_ = false;
  bool rr_ = false;
  bool rl_ = false;
  bool l_ = false;
  bool r_ = false;
  bool u_ = false;
  bool d_ = false;
  bool f_ = false;
  client& client_;

  game_drawer game_drawer_;
  };
  
#endif
