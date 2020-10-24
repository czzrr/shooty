#ifndef PLAYER_H
#define PLAYER_H

enum class player_action : uint8_t { up, down, left, right, rotate_left, rotate_right, fire_bullet };

class player_action_message
{
public:
  player_action_message() { message_.resize(1); }
  
  player_action_message(player_action pa)
  {
    message_.push_back(pa);
  }

  player_action msg()
  {
    return message_[0];
  }

  player_action* data()
  {
    return message_.data();
  }
  
private:
  std::vector<player_action> message_;
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

  return "up";
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
