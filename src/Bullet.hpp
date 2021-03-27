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
    ar & pos_.getX();
    ar & pos_.getY();
  }

  Bullet() = delete;
  
  Bullet(int x, int y, int dx, int dy) {
    pos_ = Point(x, y);
    vel_ = Velocity(x, y);
  }

  Point getPos() const {
    return pos_;
  }

  Velocity getVel() const {
    return vel_;
  }
  
  void move()
  {
    pos_.setX(pos_.getX() + vel_.getDx());
    pos_.setY(pos_.getY() + vel_.getDy());
  }

  bool operator==(const Bullet& other)
  {
    return pos_.getX() == other.getPos().getX() && pos_.getY() == other.getPos().getY();
  }

};

#endif
