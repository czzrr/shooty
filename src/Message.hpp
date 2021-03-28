#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <cstring>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <string>

template <typename T>
struct Header {
    T messageId;
  uint32_t size = 0;
};

template <typename T>
class Message {
public:

  Message() {
    body.resize(9999);
  }
               
  Header<T> header;
  std::string body;

  uint32_t headerSize() {
    return sizeof(Header<T>);
  }
  
  void setMessageId(T messageId) {
    header.messageId = messageId;
  }

  template<typename BodyData>
  void setData(BodyData data) {
    std::stringstream ss;
    {
      boost::archive::text_oarchive oa(ss);
      oa & data;
    }
    body = ss.str();
    header.size = body.size();
  }

  template <typename BodyData>
  void getData(BodyData& data) {
    std::stringstream ss;
    ss << body;
    {
      boost::archive::text_iarchive ia(ss);
      ia & data;
    }
  }

  size_t bodySize() {
    return body.size();
  }
  
};

#endif
