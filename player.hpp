#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"

#include "bullet.hpp"
#include <cmath>

#include "constants.hpp"

#include <boost/serialization/vector.hpp>

const double PI = 3.141592653589793238463;
const double DEG_TO_RAD = PI / 180.0;

class player
{
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & x_;
    ar & y_;
    ar & bullets_;
  }
  
  player() { }
  
  player(int x, int y, int id): x_(x), y_(y), id_(id) { }

  int get_x()
  {
    return x_;
  }

  int get_y()
  {
    return y_;
  }

  void move_up()
  {
    int new_y = y_ - dy_;
    if (new_y >= 0)
      y_ = new_y;
  }

  void move_down()
  {
    int new_y = y_ + dy_;
    if (new_y + PLAYER_SIDE < SCREEN_HEIGHT)
      y_ = new_y;
  }

  void move_left()
  {
    int new_x = x_ - dx_;
    if (new_x >= 0)
      x_ = new_x;
  }

  void move_right()
  {
    int new_x = x_ + dx_;
    if (new_x + PLAYER_SIDE < SCREEN_WIDTH)
      x_ += dx_;
  }

  void fire()
  {
    uint32_t ticks = SDL_GetTicks();
    if (ticks - last_time_fired_ > 200)
      {
        last_time_fired_ = ticks;
            
        double rx = 4 * cos(dir_ * DEG_TO_RAD);
        double ry = 4 * sin(dir_ * DEG_TO_RAD);
        bullets_.push_back(bullet(x_, y_, rx, ry));
      }

  }

  void rotate_left()
  {
    dir_ -= ddir_;
    // if (dir_ > 360.0)
    //   dir_ -= 360.0;
  }

  void rotate_right()
  {
    dir_ += ddir_;
    // if (dir_ < -360.0)
    //   dir_ += 360.0;
  }
  
  std::vector<bullet>& get_bullets()
  {
    return bullets_;
  }
  
private:

  uint32_t last_time_fired_ = SDL_GetTicks();
  double dir_ = 0;
  double ddir_ = 2.0;
  int id_;
  int x_;
  int y_;
  int dx_ = 5;
  int dy_ = 5;

  std::vector<bullet> bullets_;
};

enum class player_action : uint8_t { up, down, left, right, rotate_left, rotate_right, fire_bullet };

std::string get_player_action_str(player_action action)
{
  switch (action) {
  case player_action::up:
    return "up";
  case player_action::left:
    return "left";
  case player_action::down:
    return "down";
  case player_action::right:
    return "right";
  case player_action::rotate_left:
    return "rotate_left";
  case player_action::rotate_right:
    return "rotate_right";
  case player_action::fire_bullet:
    return "fire_bullet";
  }
}

class owned_player_action
{
public:
  owned_player_action(player_action action, int id): action_(action), id_(id) { }

  int get_id()
  {
    return id_;
  }
  
  player_action get_action()
  {
    return action_;
  }
  
private:    
  int id_;
  player_action action_;
};

#endif
