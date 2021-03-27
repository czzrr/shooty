#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>

#include "player.hpp"
#include "bullet.hpp"
#include "constants.hpp"

#include <boost/serialization/map.hpp>



class Game
{
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & players_;
  }
  
  Game() { }
  
  void addPlayer(int id)
  {
    // Place player on random position (and grant him 3 seconds immunity?).
    Player player(100, 100, id);
    players_.insert({id, player});
  }

  void removePlayer(int id)
  {
    auto found = players_.find(id);
    if (found != players_.end())
      {
        players_.erase(found);
      }
  }

  const std::map<int, player>& getPlayers()
  {
    return players_;
  }

  bool performAction(int id, PlayerAction playerAction)
  {
    auto found = players_.find(id);
    if (found != players_.end())
      {
        found->second.performAction(pa);
        return true;
      }
    return false;
  }

  // Advance to the next game state.
  void advance()
  {
    std::vector<int> playersToDelete;
    for (auto& [_, player] : players_)
      {
        std::vector<Bullet>& bullets = player.getBullets();
        std::vector<Bullet> bulletsToDelete;
        for (Bullet& b : player.getBullets())
          {
            for (auto [_, p] : players_)
              {
                // Check if current player's bullets collides with another player.
                if (player.getID() != p.getID() && collides(b, p))
                  {
                    playersToDelete.push_back(p.id());
                    bulletsToDelete.push_back(b);
                  }
                // Check if bullet is outside screen.
                else if (isOutsideScreen(b))
                  {
                    bulletsToDelete.push_back(b);
                  }
              }
            b.move();
          }
        // Remove bullets that have collided with players or are out of screen.
        for (bullet& b : bulletsToDelete)
          {
            bullets.erase(std::remove(bullets.begin(), bullets.end(), b), bullets.end());
          }
      }
    // Remove players hit by a bullet.
    for (int id : playersToDelete)
      {
        remove_player(id);
      }
  }
  
private:
  std::map<int, player> players_;
};


bool isOutsideScreen(Bullet bullet) {
  return bullet.x < 0 || bullet.x > SCREEN_WIDTH || bullet.y < 0 || bullet.y > SCREEN_HEIGHT;
}

bool collides(Bullet b, Player p)
{
  return collides_rect({b.get_pos().x, b.get_pos().y, BULLET_SIDE, BULLET_SIDE},
                       {p.get_pos().x, p.get_pos().y, PLAYER_SIDE, PLAYER_SIDE});
}

#endif
