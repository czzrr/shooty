#ifndef OWNED_MESSAGE_H
#define OWNED_MESSAGE_H

template <typename T>
struct OwnedMessage {
  uint32_t id;
  Message<T> msg;
};

#endif
