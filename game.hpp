#ifndef GAME_H
#define GAME_H

class game_state
{
public:
  bool game_running;
  int x, y;
};

class game_state_message
{
public:
  game_state_message(game_state pa)
  {
    message_.push_back(pa);
  }

  game_state* data()
  {
    return message_.data();
  }
  
private:
  std::vector<game_state> message_;
};

#endif
