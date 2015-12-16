add_library (TCPServer tcpserver.cpp socket.cpp data.cpp console.cpp)
target_link_libraries(TCPServer pthread)
