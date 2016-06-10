var p = nucleus.dofile('pretty-print.js').prettyPrint;
var uv = nucleus.uv;

var stdin = new uv.Tty(0, true);
var stdout = new uv.Tty(1, false);
var stderr = new uv.Tty(2, false);

function toAscii(data) {
  var str = "";
  for (var i = 0; i < data.length; i++) {
    str += String.fromCharCode(data[i]);
  }
  return str;
}

stdin.setMode(1);
stdout.write("> ");
stdin.readStart(function (err, chunk) {
  if (err) {
    stderr.write("" + err);
    return stdin.readStop();
  }
  if (chunk) {
    p(chunk, toAscii(chunk));
    if (chunk.length == 1 && chunk[0] == 0x04) {
      stdin.readStop();
    }
    stdout.write("> ");
  }
  else {
    stdin.readStop();
  }
});


uv.run();
uv.ttyResetMode();
