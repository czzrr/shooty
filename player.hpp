#ifndef PLAYER_H
#define PLAYER_H

#include "SDL.h"

#include "bullet.hpp"
#include <cmath>

#include "constants.hpp"

#include <boost/serialization/vector.hpp>

const double PI = 3.141592653589793238463;
const double DEG_TO_RAD = PI / 180.0;

enum class player_action : uint8_t { up, down, left, right, rotate_left, rotate_right, fire_bullet };

class player
{
public:

  // For (de)serialization.
  friend class boost::serialization::access;
  template<class Archive>
  void serialize(Archive& ar, const unsigned int version)
  {
    ar & pos_.x;
    ar & pos_.y;
    ar & bullets_;
  }
  
  player() { }
  
  player(int x, int y, int id) {
    pos_ = {x, y};
    id_ = id;
  }

  int id() const
  {
    return id_;
  }
  
  point get_pos()
  {
    return pos_;
  }

  void do_action(player_action pa)
  {
    switch (pa)
      {
      case player_action::up:
        move_up();
        break;

      case player_action::down:
        move_down();
        break;
        
      case player_action::left:
        move_left();        
        break;
        
      case player_action::right:
        move_right();
        break;
        
      case player_action::fire_bullet:
        fire();
        break;
        
      case player_action::rotate_left:
        rotate_left();
        break;
        
      case player_action::rotate_right:
        rotate_right();
        break;
      }
  }
  
  void move_up()
  {
    int new_y = pos_.y - vel_s.dy;
    if (new_y >= 0)
      pos_.y = new_y;
  }

  void move_down()
  {
    int new_y = pos_.y + vel_s.dy;
    if (new_y + PLAYER_SIDE < SCREEN_HEIGHT)
      pos_.y = new_y;
    
  }

  void move_left()
  {
    int new_x = pos_.x - vel_s.dx;
    if (new_x >= 0)
      pos_.x = new_x;
  }

  void move_right()
  {
    int new_x = pos_.x + vel_s.dx;
    if (new_x + PLAYER_SIDE < SCREEN_WIDTH)
      pos_.x = new_x;
  }

  void fire()
  {
    uint32_t ticks = SDL_GetTicks();
    if (ticks - last_time_fired_ > 200)
      {
        last_time_fired_ = ticks;
            
        int ball_dx = static_cast<int>(std::round(4 * cos(dir_ * DEG_TO_RAD)));
        int ball_dy = static_cast<int>(std::round(4 * sin(dir_ * DEG_TO_RAD)));
        bullets_.push_back(bullet(pos_.x, pos_.y, ball_dx, ball_dy));
      }

  }

  void rotate_left()
  {
    dir_ -= ddir_s;
  }

  void rotate_right()
  {
    dir_ += ddir_s;
  }
  
  std::vector<bullet>& get_bullets()
  {
    return bullets_;
  }
  
private:

  uint32_t last_time_fired_ = SDL_GetTicks();

  point pos_;
  int id_;
  std::vector<bullet> bullets_;
  double dir_ = 0;
  
  static constexpr double ddir_s = 2.0;
  static constexpr velocity vel_s = {5, 5};
  
  
};



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

// This can be a struct instead.
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

// Like this:
// struct owned_player_action
// {
//   owned_player_action(player_action pa, int pid): action(pa), id(pid) { }

//   int id;
//   player_action action;
// };

#endif
