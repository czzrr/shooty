#ifndef VELOCITY_H
#define VELOCITY_H

struct Velocity {
  int dx;
  int dy;

  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & dx;
    ar & dy;
  }
  
};

#endif
