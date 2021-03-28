#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <cstring>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <string>

// Header of a message.
// Contains a message ID and the size of the body.
template <typename T>
struct Header {
  T messageId;
  uint32_t size = 0;
};

// This class represents a message that can be exchanged between peers.
// It uses the Boost serialization library to encode and decode data.
// Hence the objects to be sent in the message body must implement serialization functions.
template <typename T>
struct Message {
  Header<T> header;
  std::string body;

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
  
};

#endif
