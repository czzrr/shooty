#ifndef MESSAGE_H
#define MESSAGE_H

struct s_message
{

  s_message()
  {
    header.resize(4);
  }
  
  bool set_header(int n)
  {
    // Length of header must be between 1 and 9999 inclusive.
    if (n < 1 || n > 9999)
      return false;

    header = std::to_string(n);

    // Pad with leading zeros so the header consists of 4 characters.
    std::string zeros;
    for (int i = 4 - header.size(); i > 0; i--)
      zeros += "0";
    zeros += header;
    header = zeros;

    header_n = n;
    body.resize(header_n);
    
    return true;
  }

  static int parse_header(std::string h)
  {
    return std::stoi(h);
  }

  int header_n;
  std::string header;
  std::string body;
};
  
#endif
