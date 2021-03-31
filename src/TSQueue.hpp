#ifndef TSQUEUE_H
#define TSQUEUE_H

#include <iostream>

#include <queue>
#include <mutex>
#include <condition_variable>

template <typename T>
class TSQueue {
  std::queue<T> queue_;
  std::mutex mutex_;
  std::condition_variable cond_;

public:
  void push(T data) {
    std::scoped_lock guard(mutex_);
    queue_.push(data);
  }

  T& front() {
    std::scoped_lock guard(mutex_);
    return queue_.front();
  }
  
  T pop() {
    std::scoped_lock guard(mutex_);
    T data = queue_.front();
    queue_.pop();
    return data;
  }
  
  bool empty() {
    std::scoped_lock guard(mutex_);
    return queue_.empty();
  }
    
};


#endif
