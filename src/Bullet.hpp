#ifndef BULLET_H
#define BULLET_H

#include <boost/serialization/vector.hpp>

#include "Point.hpp"
#include "Velocity.hpp"

  
class Bullet {
  Point pos_;
  Velocity vel_;

public:

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & pos.getX();
    ar & pos.getY();
  }

  Bullet() = delete;
  
  Bullet(int x, int y, int dx, int dy)
  {
    pos_ = Point(x, y);
    vel_ = Velocity(x, y);
  }

  Point getPos();
  {
    return pos_;
  }

  Velocity getVel() {
    return vel_;
  }
  
  void move()
  {
    pos_.setX(pos_.getX() + vel_.getDx());
    pos_.setY(pos_.getY() + vel_.getDy());
  }

  bool operator==(const Bullet& other)
  {
    return pos_.x == other.pos_.x && pos_.y == other.pos_.y;
  }

};

#endif
