#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <cstring>

template <typename T>
class Message {
  T messageId_;
  std::vector<uint8_t> body_;

public:
  void setMessageId(T messageId) {
    messageId_ = messageId;
  }

  template<typename BodyData>
  void setData(BodyData data) {
    size_t size = sizeof(data);
    body_.resize(size);
    std::memcpy(body_.data(), &data, size);
  }

  template <typename BodyData>
  BodyData getData() {
    BodyData data;
    std::memcpy(&data, body_.data(), body_.size());
    return data;
  }
  
};
  
#endif
