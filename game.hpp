#ifndef GAME_H
#define GAME_H

#include "player.hpp"
#include "bullet.hpp"

class game
{
public:
  game()
  {

  }
  
  void add_player(player p)
  {
   players_.push_back(p);
  }

  const std::vector<player>& get_players()
  {
    return players_;
  }

  void move_player_up(int id)
  {
    players_[id].move_up();
  }

  void move_player_down(int id)
  {
    players_[id].move_down();
  }

  void move_player_left(int id)
  {
    players_[id].move_left();
  }

  void move_player_right(int id)
  {
    players_[id].move_right();
  }

  void player_fire(int id)
  {
    players_[id].fire();
  }

  void player_rotate_right(int id)
  {
    players_[id].rotate_right();
  }

  void player_rotate_left(int id)
  {
    players_[id].rotate_left();
  }
  
  void advance()
  {
    for (player& player : players_)
      
      {
        for (bullet& bullet : player.get_bullets())
          {
            bullet.move();
          }
      }
  }
  
private:
  std::vector<player> players_;
  //std::vector<bullet> bullets_;
};

#endif
