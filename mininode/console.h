#ifndef _CONSOLE_H_
#define _CONSOLE_H_

#include <string>

class Console {
public:
  void log(std::string msg);
  void err(std::string msg);
};

#ifndef NO_CONSOLE_OBJECT
  Console console;
#endif

#endif
