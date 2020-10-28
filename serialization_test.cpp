#include <iostream>
#include <fstream>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/vector.hpp>

#include "player.hpp"
#include "game.hpp"

#include "SDL.h"

int main()
{
  game g;
  
  player p1(5, 10, 1);
  
  SDL_Delay(1000);
  
  p1.fire();
  p1.move_down();
  p1.move_right();
  p1.move_right();
  g.add_player(p1);

  player p2(17, 19, 2);
  p2.move_left();
  g.add_player(p2);

  std::cout << "before\n";
  for (player p : g.get_players())
    {
      std::cout << "x: " << p.get_x() << ", y: " << p.get_y() << "\n";

      for (bullet b : p.get_bullets())
        {
          std::cout << "bullet. x: " << b.get_x() << ", y: " << b.get_y() << "\n";
        }

    }



  std::stringstream ss;
  {
    boost::archive::text_oarchive oa(ss);
    oa & g;
  }

  std::cout << "\n string view :\n" << ss.str() << "\n string size: " << ss.str().size();
    
  game newg;

  {
    boost::archive::text_iarchive ia(ss);
    ia & newg;
  }

  std::cout << "after\n";
  for (player p : newg.get_players())
    {
      std::cout << "x: " << p.get_x() << ", y: " << p.get_y() << "\n";

      for (bullet b : p.get_bullets())
        {
          std::cout << "bullet. x: " << b.get_x() << ", y: " << b.get_y() << "\n";
        }

    }

    
    
      
}
