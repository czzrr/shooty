#include "TSQueue.hpp"
#include <iostream>

void printQ(TSQueue<int>& q) {
  while (!q.empty()) {
    int x = q.waitAndPop();
    std::cout << x << " ";
  }
}

int main() {
  TSQueue<int> q;
  q.push(4);
  q.push(7);
  q.push(11);
  q.push(23);
  q.waitAndPop();
  printQ(q);
}
