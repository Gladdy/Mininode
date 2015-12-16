var net = require('net');

var server = net.createServer(function(socket){

  console.log('A client connected');
  console.log("address: " + socket.remoteAddress);
  console.log("port:    " + socket.remotePort);

  i = 1;
  socket.on('data', function(data){
    i = i + 1;
    console.log(i);
  });

  socket.on('end', function(){
    console.log("Client disconnected");
  });
});

server.listen(1338);
