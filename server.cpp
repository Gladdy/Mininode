#include "nodejs.h"

#include <map>
#include <iostream>

struct ConnectionData {
	std::string username;
	bool providedUsername = false;
	int messagecount = 0;
};

int main()
{
	std::map<Socket, ConnectionData> connections;

	TCPServer server ([&](Socket &socket) {

		//connections.insert(std::make_pair(socket, ConnectionData()));
		connections[socket] = ConnectionData();

		console.log("Client connected");
		console.log("address: " + socket.remoteAddress());
		console.log("port:    " + socket.remotePort());

		socket.write("Please provide a user name: ");

		socket.on("data", [&, socket](Data data) mutable {
			ConnectionData& cd = connections.at(socket);

			// REGISTER A USERNAME
			if(!cd.providedUsername) {
				std::string username = std::string(data);

				if(username.back() == '\n') {
					username.pop_back();
				}

				cd.username = username;
				cd.providedUsername = true;
				socket.write("Username registered! ");
				socket.write("Welcome " + cd.username);
				return;
			}

			// CLOSE THE CONNECTION WHEN PROVIDED WITH "exit"
			std::string content = data;
			if(content.substr(0,4) == "exit") {
				socket.close();
				connections.erase(socket);
				return;
			}

			// BROADCAST THE MESSAGE TO ALL OTHER CONNECTED SOCKETS
			for(const auto &connection : connections) {
	    	if(connection.first != socket) {
	    		connection.first.write("");
	    		connection.first.write(cd.username);
	    		connection.first.write(content);
	    	}
	    }
	  });

		socket.on("end", [&, socket](){
			console.log("Client disconnected");
		});
	});
	server.listen(1338);

	TCPServer::block();
	return 0;
}
