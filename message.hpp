#ifndef MESSAGE_H
#define MESSAGE_H

template<typename T>
struct message
{
  std::vector<uint8_t> body;

  message()
  {
    body.resize(sizeof(T));
  }
  
  message(T data)
  {
    body.resize(sizeof(T));
    std::memcpy(body.data(), &data, sizeof(T));
  }

  size_t size()
  {
    return sizeof(T);
  }
};

#endif
