#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>

#include "player.hpp"
#include "bullet.hpp"
#include "constants.hpp"

#include <boost/serialization/map.hpp>

bool collides(bullet b, player p)
{
  return collides_rect({b.get_pos().x, b.get_pos().y, BULLET_SIDE, BULLET_SIDE},
                       {p.get_pos().x, p.get_pos().y, PLAYER_SIDE, PLAYER_SIDE});
}

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
  
  void add_player(int id)
  {
    // Place player on random position (and grant him 3 seconds immunity?).
    player p(100, 100, id);
    players_.insert({id, p});
  }

  void remove_player(int id)
  {
    auto found = players_.find(id);
    if (found != players_.end())
      {
        players_.erase(found);
      }
  }

  const std::map<int, player>& get_players()
  {
    return players_;
  }

  bool do_action(int id, player_action pa)
  {
    auto found = players_.find(id);
    if (found != players_.end())
      {
        found->second.do_action(pa);
        return true;
      }
    return false;
  }

  // Advance to the next game state.
  void advance()
  {
    std::vector<int> players_to_delete;
    for (auto& [_, player] : players_)
      {
        std::vector<bullet>& bullets = player.get_bullets();
        std::vector<bullet> bullets_to_delete;
        for (bullet& b : player.get_bullets())
          {
            for (auto [_, p] : players_)
              {
                // Check if current player's bullets collides with another player.
                if (player.id() != p.id() && collides(b, p))
                  {
                    players_to_delete.push_back(p.id());
                    bullets_to_delete.push_back(b);
                  }
                // Check if bullet is outside screen.
                else if (b.outside_screen())
                  {
                    bullets_to_delete.push_back(b);
                  }
              }
            b.move();
          }
        // Remove bullets that have collided with players or are out of screen.
        for (bullet& b : bullets_to_delete)
          {
            bullets.erase(std::remove(bullets.begin(), bullets.end(), b), bullets.end());
          }
      }
    // Remove players hit by a bullet.
    for (int id : players_to_delete)
      {
        remove_player(id);
      }
  }
  
private:
  // TODO: Have a map<int, player> instead. Also , player objects don't need an id; an id for the connection is enough.
  std::map<int, player> players_;
  //std::vector<player> players_;
  //std::vector<bullet> bullets_;
};

#endif
