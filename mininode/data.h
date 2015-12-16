#ifndef DATA_H_
#define DATA_H_

#include <string>

class Data {
public:
  Data(std::string);
  operator std::string() const;
private:
  std::string content;
};

#endif
