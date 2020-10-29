#ifndef MESSAGE_H
#define MESSAGE_H

#include <vector>
#include <array>
#include <string>
#include <cstring>

#include <iostream>

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

  size_t size() const
  {
    return sizeof(T);
  }
};

struct s_message
{

  s_message()
  {
    header.resize(4);
    body.resize(9999);
  }
  
  bool set_header(int n)
  {
    if (n < 0 || n > 9999)
      return false;

    header = std::to_string(n);

    std::string zeros;
    for (int i = 4 - header.size(); i > 0; i--)
      zeros += "0";
    zeros += header;
    header = zeros;

    //std::cout << "header: " << header << "\n";
    header_n = n;
    return true;
  }

  static int parse_header(std::string h)
  {
    return std::stoi(h);
  }

  int header_n;
  std::string header;
  std::string body;
  std::string concatted;
};
  
#endif
