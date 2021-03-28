#ifndef BULLET_H
#define BULLET_H

#include <boost/serialization/vector.hpp>

#include "Point.hpp"
#include "Velocity.hpp"


#include <iostream>
  
class Bullet {
  Point pos_;
  Velocity vel_;

public:

  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & pos_;
  }

  Bullet() {}
  
  Bullet(int x, int y, int dx, int dy) {
    pos_ = {x, y};
    vel_ = {dx, dy};
  }

  Point getPos() const {
    return pos_;
  }

  Velocity getVel() const {
    return vel_;
  }
  
  void move() {
    pos_.x = pos_.x + vel_.dx;
    pos_.y = pos_.y + vel_.dy;
  }

  bool operator==(const Bullet& other) {
    return pos_.x == other.getPos().x && pos_.y == other.getPos().y;
  }

};

#endif
