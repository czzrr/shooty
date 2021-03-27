#ifndef VELOCITY_H
#define VELOCITY_H

class Velocity {
  int dx_;
  int dy_;

public:
  Velocity() {}
  
  Velocity(int dx, int dy) {
    dx_ = dx;
    dy_ = dy;
  }

  int getDx() {
    return dx_;
  }

  int getDy() {
    return dy_;
  }

  void setDx(int dx) {
    dx_ = dx;
  }

  void setDy(int dy) {
    dy_ = dy;
  }
  
};

#endif
