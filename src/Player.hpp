#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <boost/serialization/vector.hpp>

#include "Point.hpp"
#include "Velocity.hpp"
#include "Bullet.hpp"
#include "Utils.hpp"
#include "PlayerAction.hpp"

class Player {
  uint32_t id_;
  Point pos_;
  std::vector<Bullet> bullets_;
  double angle_ = 0.0;

  Velocity vel_ = {5, 5};
  static constexpr double dAngle_ = 2.0;
  
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version) {
    ar & pos_;
    ar & bullets_;
  }
  
  Player() {}
  
  Player(int x, int y, int id) {
    pos_ = {x, y};
    id_ = id;
  }

  int getID() const {
    return id_;
  }
  
  Point getPos() const {
    return pos_;
  }

  void performAction(PlayerAction playerAction) {
    switch (playerAction)
      {
      case PlayerAction::Up:
        moveUp();
        break;

      case PlayerAction::Down:
        moveDown();
        break;
        
      case PlayerAction::Left:
        moveLeft();        
        break;
        
      case PlayerAction::Right:
        moveRight();
        break;
        
      case PlayerAction::FireBullet:
        fire();
        break;
        
      case PlayerAction::RotateLeft:
        rotateLeft();
        break;
        
      case PlayerAction::RotateRight:
        rotateRight();
        break;
      }
  }
  
  void moveUp() {
    int newY = pos_.y - vel_.dy;
    if (newY >= 0)
      pos_.y = newY;
  }

  void moveDown() {
    int newY = pos_.y + vel_.dy;
    if (newY + PLAYER_SIDE < SCREEN_HEIGHT)
      pos_.y = newY;
  }

  void moveLeft() {
    int newX = pos_.x - vel_.dx;
    if (newX >= 0)
      pos_.x = newX;
  }

  void moveRight() {
    int newX = pos_.x + vel_.dx;
    if (newX + PLAYER_SIDE < SCREEN_WIDTH)
      pos_.x = newX;
  }

  void fire() {
    int dx = static_cast<int>(std::round(4 * cos(angle_ * DEG_TO_RAD)));
    int dy = static_cast<int>(std::round(4 * sin(angle_ * DEG_TO_RAD)));
    bullets_.push_back(Bullet(pos_.x, pos_.y, dx, dy));

  }

  void rotateLeft() {
    angle_ -= dAngle_;
  }

  void rotateRight() {
    angle_ += dAngle_;
  }
  
  std::vector<Bullet>& getBullets() {
    return bullets_;
  }
};




#endif
