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
  Point pos_:
  Velocity vel_ = Velocity(5, 5);
  std::vector<Bullet> bullets_;
  double dir_ = 0;
  
  static const double dAngle = 2.0;
  
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & x;
    ar & y;
    ar & bullets_;
  }
  
  player() = delete;
  
  player(int x, int y, int id) {
    pos_ = Point(x, y);
    id_ = id;
  }

  int getID() const
  {
    return id_;
  }
  
  Point getPos()
  {
    return pos_;
  }

  void performAction(PlayerAction playerAction)
  {
    switch (playerAction)
      {
      case PlayerAction::up:
        moveUp();
        break;

      case PlayerAction::down:
        moveDown();
        break;
        
      case PlayerAction::left:
        moveLeft();        
        break;
        
      case PlayerAction::right:
        moveRight();
        break;
        
      case PlayerAction::fire_bullet:
        fire();
        break;
        
      case PlayerAction::rotate_left:
        rotateLeft();
        break;
        
      case PlayerAction::rotate_right:
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
    int newY = pos_.getY() + vel_.getDy;
    if (newY + PLAYER_SIDE < SCREEN_HEIGHT)
      pos_.setY(newY);
    
  }

  void moveLeft()
  {
    int newX = pos.getX() - vel_.getDx;
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
    int dx = static_cast<int>(std::round(4 * cos(dir_ * DEG_TO_RAD)));
    int dy = static_cast<int>(std::round(4 * sin(dir_ * DEG_TO_RAD)));
    bullets_.push_back(bullet(pos_.getX(), pos_.getY(), dx, dy));

  }

  void rotateLeft()
  {
    angle_ -= dAngle_;
  }

  void rotateRight()
  {
    angle_ += dAngle_;
  }
  
  std::vector<bullet>& getBullets()
  {
    return bullets_;
  }
};



std::string playerActionToStr(PlayerAction action)
{
  switch (action) {
  case PlayerAction::up:
    return "up";
  case PlayerAction::left:
    return "left";
  case PlayerAction::down:
    return "down";
  case PlayerAction::right:
    return "right";
  case PlayerAction::rotate_left:
    return "rotate_left";
  case PlayerAction::rotate_right:
    return "rotate_right";
  case PlayerAction::fire_bullet:
    return "fire_bullet";
  }
}
