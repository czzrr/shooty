#include "Message.hpp"
#include <iostream>

int main() {
  Message<int> msg;
  msg.setData(42);
  int x;
  msg.getData(x);
  
  std::cout << x << "\n";

  int y = msg.getData();

  
  return 0;
}
