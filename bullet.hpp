#ifndef BULLET_H
#define BULLET_H

#include "constants.hpp"

#include <boost/serialization/vector.hpp>

struct point
{
  int x;
  int y;
};

struct velocity
{
  int dx;
  int dy;
};
  
class bullet
{
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & pos_.x;
    ar & pos_.y;
  }

  bullet() { }
  
  bullet(int x, int y, int dx, int dy)
  {
    pos_ = {x, y};
    vel_ = {dx, dy};
  }

  bool outside_screen()
  {
    return pos_.x < 0 || pos_.x > SCREEN_WIDTH || pos_.y < 0 || pos_.y > SCREEN_HEIGHT;
  }

  point get_pos()
  {
    return pos_;
  }
  
  void move()
  {
    pos_.x += vel_.dx;
    pos_.y += vel_.dy;
  }

  bool operator==(const bullet& other)
  {
    return pos_.x == other.pos_.x && pos_.y == other.pos_.y;
  }

private:
  point pos_;
  velocity vel_;
};

#endif
