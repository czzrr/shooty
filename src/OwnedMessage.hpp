#ifndef OWNED_MESSAGE_H
#define OWNED_MESSAGE_H

template <typename T>
class OwnedMessage : Message<T> {
public:
  int id_;
};

#endif
