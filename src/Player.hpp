#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <boost/serialization/vector.hpp>

#include "Point.hpp"
#include "Velocity.hpp"
#include "Bullet.hpp"
#include "Constants.hpp"

enum class PlayerAction : uint8_t { Up, Down, Left, Right, RotateLeft, RotateRight, FireBullet };

class Player {
  uint32_t id_;
  Point pos_;
  std::vector<Bullet> bullets_;
  double angle_ = 0;

  Velocity vel_ = Velocity(5, 5);
  static constexpr double dAngle_ = 2.0;
  
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & pos_.getX();
    ar & pos_.getY();
    ar & bullets_;
  }
  
  Player() = delete;
  
  Player(int x, int y, int id) {
    pos_ = Point(x, y);
    id_ = id;
  }

  int getID() const {
    return id_;
  }
  
  Point getPos() const {
    return pos_;
  }

  void performAction(PlayerAction playerAction)
  {
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
  
  void moveUp()
  {
    int newY = pos_.getY() - vel_.getDy();
    if (newY >= 0)
      pos_.setY(newY);
  }

  void moveDown()
  {
    int newY = pos_.getY() + vel_.getDy();
    if (newY + PLAYER_SIDE < SCREEN_HEIGHT)
      pos_.setY(newY);
    
  }

  void moveLeft()
  {
    int newX = pos_.getX() - vel_.getDx();
    if (newX >= 0)
      pos_.setX(newX);
  }

  void moveRight()
  {
    int newX = pos_.getX() + vel_.getDx();
    if (newX + PLAYER_SIDE < SCREEN_WIDTH)
      pos_.setX(newX);
  }

  void fire()
  {
    int dx = static_cast<int>(std::round(4 * cos(angle_ * DEG_TO_RAD)));
    int dy = static_cast<int>(std::round(4 * sin(angle_ * DEG_TO_RAD)));
    bullets_.push_back(Bullet(pos_.getX(), pos_.getY(), dx, dy));

  }

  void rotateLeft()
  {
    angle_ -= dAngle_;
  }

  void rotateRight()
  {
    angle_ += dAngle_;
  }
  
  std::vector<Bullet>& getBullets()
  {
    return bullets_;
  }
};



std::string playerActionToStr(PlayerAction action)
{
  switch (action) {
  case PlayerAction::Up:
    return "up";
  case PlayerAction::Left:
    return "left";
  case PlayerAction::Down:
    return "down";
  case PlayerAction::Right:
    return "right";
  case PlayerAction::RotateLeft:
    return "rotate_left";
  case PlayerAction::RotateRight:
    return "rotate_right";
  case PlayerAction::FireBullet:
    return "fire_bullet";
  }
}

#endif
