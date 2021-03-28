#ifndef UTILS_H
#define UTILS_H

#include "SDL.h"

const int SCREEN_WIDTH = 1000;
const int SCREEN_HEIGHT = 1000;

const int PLAYER_SIDE = 15;
const int BULLET_SIDE = 5;

const int FRAMES_PER_SECOND = 60;

const double PI = 3.141592653589793238463;
const double DEG_TO_RAD = PI / 180.0;

bool collidesRect(SDL_Rect r1, SDL_Rect r2) {
  int left_r1 = r1.x;
  int right_r1 = r1.x + r1.w;
  int top_r1 = r1.y;
  int bottom_r1 = r1.y + r1.h;

  int left_r2 = r2.x;
  int right_r2 = r2.x + r2.w;
  int top_r2 = r2.y;
  int bottom_r2 = r2.y + r2.h;

  if (right_r1 <= left_r2)
    return false;

  if (right_r2 <= left_r1)
    return false;

  if (bottom_r1 <= top_r2)
    return false;

  if (bottom_r2 <= top_r1)
    return false;

  return true;
}


#endif
