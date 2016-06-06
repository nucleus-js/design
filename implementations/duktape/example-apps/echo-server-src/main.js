// Load a library from a file using the native dofile primitive directly.
var p = nucleus.dofile("pretty-print.js").prettyPrint;
// Load the uv module in the local scope.
var uv = nucleus.uv;

print("Starting local TCP echo server...");

var server = new uv.Tcp();
server.bind("127.0.0.1", 1337);
server.listen(128, onConnection);
print("Server is now bound and listening for new connections...");
p(server.getsockname());

print("Starting libuv event loop");
uv.run();
print("Event loop exited");

// function statements for callbacks can go anwehere in JS.

function onConnection(err) {
  if (err) throw err;
  var client = new uv.Tcp();
  server.accept(client);
  client.readStart(onRead);

  print("New TCP client accepted");
  p(client.getpeername());

  function onRead(err, chunk) {
    if (err) throw err;
    if (chunk) {
      print("Echoing chunk back to client...");
      p(chunk);
      client.write(chunk);
    }
    else {
      print("received EOF from client, shutting down...");
      client.shutdown(function () {
        client.close();
        server.close();
      });
    }
  }
}
