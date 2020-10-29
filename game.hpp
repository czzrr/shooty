#ifndef GAME_H
#define GAME_H

#include <iostream>

#include "player.hpp"
#include "bullet.hpp"
#include <vector>

#include <boost/serialization/vector.hpp>

class game
{
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & players_;
  }
  
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

  // Advance to the next game state.
  void advance()
  {
    for (player& player : players_)
      {
        std::vector<bullet>& bullets = player.get_bullets();
        bool bullet_outside_screen = false;
        for (bullet& bullet : bullets)
          {
            if (bullet.outside_screen())
              {
                bullet_outside_screen = true;
              }
            
            bullet.move();
          }

        if (bullet_outside_screen)
          {
            bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](bullet b) { return b.outside_screen(); }));
          }
      }
  }
  
private:
  std::vector<player> players_;
  //std::vector<bullet> bullets_;
};

#endif
