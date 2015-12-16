#define NO_CONSOLE_OBJECT

#include "console.h"
#include <iostream>

void Console::log(std::string msg) {
  std::cout << msg << std::endl;

  // if(msg[msg.size()-1] != '\n') {
  //   std::cout << std::endl;
  // }
}

void Console::err(std::string msg) {
  std::cerr << msg << std::endl;

  // if(msg[msg.size()-1] != '\n') {
  //   std::cerr << std::endl;
  // }
}
