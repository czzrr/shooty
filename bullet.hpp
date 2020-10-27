#ifndef BULLET_H
#define BULLET_H

class bullet
{
public:
  bullet(int x, int y, double dx, double dy)
  {
    x_ = x;
    y_ = y;
    dx_ = dx;
    dy_ = dy;
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
