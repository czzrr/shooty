#include "message.hpp"

#include <iostream>

int main()
{
  s_message msg;
  int n = 123;
  std::cout << msg.parse_header("0012") << "\n";
}
