#ifndef BULLET_H
#define BULLET_H

#include "constants.hpp"

#include <boost/serialization/vector.hpp>

class bullet
{
public:

  friend class boost::serialization::access;
  
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & x_;
    ar & y_;
  }

  bullet() { }
  
  bullet(int x, int y, double dx, double dy)
  {
    x_ = x;
    y_ = y;
    dx_ = dx;
    dy_ = dy;
  }

  bool outside_screen()
  {
    return x_ < 0 || x_ > SCREEN_WIDTH || y_ < 0 || y_ > SCREEN_HEIGHT;
  }
  
  int get_x()
  {
    return x_;
  }

  int get_y()
  {
    return y_;
  }

  void move()
  {
    x_ += (int) dx_;
    y_ += (int) dy_;
  }
  
private:
  int x_;
  int y_;
  double dx_;
  double dy_;
};

#endif
