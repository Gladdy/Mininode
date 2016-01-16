#include "socket.h"
#include "data.h"
#include "tcpserver.h"

#include <iostream>
#include <algorithm>

#include <netdb.h>
#include <sys/fcntl.h>

#include <iostream>
#include <unistd.h>
#include <cstring>
#include <errno.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>

Socket::Socket(int fd, std::string a, std::string p) :
	_remoteAddress(a),
	_remotePort(p),
	fileDescriptor(fd)
{
  dataFunction = [](Data data){};
  endFunction = [](){};
}

void Socket::on(std::string str, std::function<void()> f) {
	if(str == "end") { endFunction = f; }
}
void Socket::on(std::string str, std::function<void(Data)> f) {
	if(str == "data") { dataFunction = f; }
}

void Socket::write(std::string str) const {
	write(Data(str));
}
void Socket::write(Data data) const {

	std::string content = data;
	content.push_back('\n');
	char writebuffer[4096];

	int totalToSend = content.size();
	int currentPosition = 0;

	while((currentPosition < totalToSend) && currentPosition !=-1) {

		int bufferContentSize = std::min(4096, totalToSend - currentPosition);
		content.copy(writebuffer, bufferContentSize, currentPosition);

		ssize_t sent = send(fileDescriptor, writebuffer, bufferContentSize, 0);
		currentPosition = currentPosition + sent;
	}
}
void Socket::write(Data data, std::function<void()> f) const {

}

void Socket::close() {
	::close(fileDescriptor);
}
void Socket::close(std::function<void()> f) {

}

std::string Socket::remoteAddress() const {
	return _remoteAddress;
}
std::string Socket::remotePort() const {
	return _remotePort;
}

bool Socket::operator == (const Socket &ref) const
{
    return(this->fileDescriptor == ref.fileDescriptor);
}
bool Socket::operator != (const Socket &ref) const
{
    return(this->fileDescriptor != ref.fileDescriptor);
}

bool Socket::operator < (const Socket &ref) const
{
    return(this->fileDescriptor < ref.fileDescriptor);
}
