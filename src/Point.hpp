#ifndef POINT_H
#define POINT_H

struct Point {
  int x;
  int y;
  
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & x;
    ar & y;
  }
};

#endif
