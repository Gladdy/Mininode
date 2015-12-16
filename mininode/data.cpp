#include "data.h"

Data::Data(std::string str) :
  content(str)
{

}

Data::operator std::string() const {
  return content;
}
