#ifndef PLAYER_ACTION_H
#define PLAYER_ACTION_H

enum class PlayerAction : uint8_t { Up, Down, Left, Right, RotateLeft, RotateRight, FireBullet };

std::string playerActionToStr(PlayerAction action)
{
  switch (action) {
  case PlayerAction::Up:
    return "up";
  case PlayerAction::Left:
    return "left";
  case PlayerAction::Down:
    return "down";
  case PlayerAction::Right:
    return "right";
  case PlayerAction::RotateLeft:
    return "rotate_left";
  case PlayerAction::RotateRight:
    return "rotate_right";
  case PlayerAction::FireBullet:
    return "fire_bullet";
  }
  return "";
}


#endif
