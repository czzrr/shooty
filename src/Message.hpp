#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <cstring>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <string>

template <typename T>
class Message {
  T messageId_;
  std::string body_;

public:
  void setMessageId(T messageId) {
    messageId_ = messageId;
  }

  template<typename BodyData>
  void setData(BodyData data) {
    std::stringstream ss;
    {
      boost::archive::text_oarchive oa(ss);
      oa & data;
    }
    body_ = ss.str();
  }

  template <typename BodyData>
  void getData(BodyData& data) {
    std::stringstream ss;
    ss << body_;
    {
      boost::archive::text_iarchive ia(ss);
      ia & data;
    }
  }
  
};

#endif
