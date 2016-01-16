#ifndef TCPSERVER_H_
#define TCPSERVER_H_

#include "socket.h"
#include "data.h"

#include <functional>
#include <vector>
#include <thread>
#include <netdb.h>
#include <mutex>


class TCPServer {
	friend class Socket;

public:
	TCPServer(std::function<void(Socket&)>);
	void listen(uint16_t);
	static void block();

private:
	std::function<void(Socket&)> socketFunction;

	uint16_t port;

	std::thread acceptingThread;
	void acceptConnections();

	std::thread processingThread;
	void processConnections();

	std::vector<Socket> socketVector;

	static uint32_t activeCounter;
	std::mutex vectMutex;
};



#endif
