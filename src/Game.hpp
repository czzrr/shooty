#ifndef GAME_H
#define GAME_H

#include <iostream>
#include <vector>

#include "Player.hpp"
#include "Bullet.hpp"
#include "Utils.hpp"
#include <chrono>
#include <map>
#include <boost/serialization/map.hpp>

bool isOutsideScreen(Bullet bullet);
bool collides(Bullet b, Player p);

// The Game class keeps track of the game state.
class Game {
  std::map<uint32_t, Player> players_;
  std::map<uint32_t, std::chrono::time_point<std::chrono::system_clock>> lastBulletTimes_;
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & players_;
  }
  
  Game() { }

  int getNumPlayers() {
    return players_.size();
  }

  // Synchronize players with connections if either a player has disconnected
  // or a player has connected. Dead players are handled in the advance() function.
  void syncPlayers(std::vector<uint32_t> ids) {
    // If player has connected
    if (players_.size() < ids.size()) {
      addPlayer(ids[ids.size() - 1]); // Add a player with the connection's ID
      // If player has disconnected
    } else if (players_.size() > ids.size()) {
      // Remove players with no corresponding active connection
      for (auto& [id, _] : players_) {
        if (std::find(ids.begin(), ids.end(), id) == ids.end()) {
          removePlayer(id);
          break;
        }
      }
    }
  }

  void addPlayer(uint32_t id) {
    //std::cout << "Adding player with ID " << id << "\n";
    Player player(100, 100, id);
    players_.insert({id, player});
  }

  void removePlayer(uint32_t id) {
    //std::cout << "Removing player with ID " << id << "\n";
    auto found = players_.find(id);
    if (found != players_.end()) {
        players_.erase(found);
      }
  }

  const std::map<uint32_t, Player>& getPlayers() {
    return players_;
  }

  // Perform player action on player with matching ID
  bool performAction(uint32_t id, PlayerAction playerAction) {
    auto found = players_.find(id);
    if (found != players_.end()) {
      Player& p = found->second;
      switch (playerAction) {
      case PlayerAction::Up:
        p.moveUp();
        break;

      case PlayerAction::Down:
        p.moveDown();
        break;
        
      case PlayerAction::Left:
        p.moveLeft();        
        break;
        
      case PlayerAction::Right:
        p.moveRight();
        break;
        
      case PlayerAction::FireBullet:
        {
          // Find time when last bullet was fired
          auto foundTime = lastBulletTimes_.find(id);
          if (foundTime != lastBulletTimes_.end()) { // If found
            auto start = foundTime->second;
            auto end = std::chrono::system_clock::now();
            // Check if last bullet was fired at least 250 ms ago
            if (std::chrono::duration_cast<std::chrono::milliseconds>(end - start) >= std::chrono::milliseconds(250)) {
              lastBulletTimes_.insert_or_assign(id, end);
              p.fire();
            }
          } else { // First bullet fired, add time to map
            lastBulletTimes_.insert({id, std::chrono::system_clock::now()});
            p.fire();
          }
        }
        break;
        
      case PlayerAction::RotateLeft:
        p.rotateLeft();
        break;
        
      case PlayerAction::RotateRight:
        p.rotateRight();
        break;
      }
      return true;
    }
    return false;
  }
  
  // Advance to the next game state.
  std::vector<uint32_t> advance() {
    // Check each player's bullets to see if they collide with another player.
    // If so, remove both the bullet and the player hit.
    // Move remaining bullets.
    std::vector<uint32_t> playersToDelete;
    for (auto& [_, player] : players_) {
      std::vector<Bullet>& bullets = player.getBullets();
      std::vector<Bullet> bulletsToDelete;
      for (Bullet& b : bullets) {
        bool bulletDeleted = false;
        for (auto [_, p] : players_) {
          // Check if current player's bullets collides with another player.
          if (player.getID() != p.getID() && collides(b, p)) {
            playersToDelete.push_back(p.getID());
            bulletsToDelete.push_back(b);
            bulletDeleted = true;
          }
          // Check if bullet is outside screen.
          else if (isOutsideScreen(b)) {
            bulletsToDelete.push_back(b);
            bulletDeleted = true;
          }
        }
        if (!bulletDeleted)
          b.move();
      }
      // Remove bullets that have collided with players or are out of screen.
      for (Bullet& b : bulletsToDelete) {
        bullets.erase(std::remove(bullets.begin(), bullets.end(), b), bullets.end());
      }
    }
    // Remove players hit by a bullet.
    for (int id : playersToDelete) {
      removePlayer(id);
    }
    
    return playersToDelete;
  }
  

};

bool isOutsideScreen(Bullet bullet) {
  int x = bullet.getPos().x;
  int y = bullet.getPos().y;
  return x < 0 || x > SCREEN_WIDTH || y < 0 || y > SCREEN_HEIGHT;
}

bool collides(Bullet b, Player p) {
  return collidesRect({b.getPos().x, b.getPos().y, BULLET_SIDE, BULLET_SIDE},
                      {p.getPos().x, p.getPos().y, PLAYER_SIDE, PLAYER_SIDE});
}


#endif
