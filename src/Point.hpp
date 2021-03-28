#ifndef POINT_H
#define POINT_H

class Point {
  int x_;
  int y_;

public:

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & x_;
    ar & y_;
  }
  
  Point() {}
  
  Point(int x, int y) {
    x_ = x;
    y_ = y;
  }

  int getX() {
    return x_;
  }

  int getY() {
    return y_;
  }

  void setX(int x) {
    x_ = x;
  }

  void setY(int y) {
    y_ = y;
  }
  
};

#endif
