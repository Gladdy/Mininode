#ifndef SOCKET_H_
#define SOCKET_H_

#include <functional>
#include <netdb.h>

#include "data.h"

class Socket {
	friend class TCPServer;
public:
	void on(std::string, std::function<void(Data)>);
	void on(std::string, std::function<void()>);

	void write(std::string) const;
	void write(Data data) const;
	void write(Data data, std::function<void()>) const;

	void close();
	void close(std::function<void()>);

	std::string remoteAddress() const;
	std::string remotePort() const;

  bool operator == (const Socket &ref) const;
  bool operator != (const Socket &ref) const;
  bool operator < (const Socket &ref) const;

	int fileDescriptor;

	bool enteredUsername = false;
	std::string username;

private:

	std::string _remoteAddress;
	std::string _remotePort;

	std::function<void(Data)> dataFunction;
	std::function<void()> endFunction;

	Socket(int, std::string, std::string);

	char buffer[4096];
};

#endif
