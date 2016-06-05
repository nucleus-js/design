print("\nTesting dofile by loading pretty-printer library from another file");
var p = nucleus.dofile("deps/utils.js").prettyPrint;
p(p);

print("\nTesting pretty printer by dumping nucleus");
p(nucleus);

var uv = nucleus.uv;

print("\nTimer.prototype");
p(uv.Timer.prototype);
var timer = new uv.Timer();

print("\nTesting uv.walk");
uv.walk(p);
timer.close();

print("\nTesting simple timeout");
var timer = new uv.Timer();
timer.start(1000, 0, function () {
  p("timeout", timer);
  timer.close(function () {
    p("closed", timer);
  });
});
uv.run();

print("\nTesting simple interval");
timer = new uv.Timer();
var count = 3;
timer.start(500, 500, function () {
  p("interval", timer);
  if (--count) return;
  timer.close(function () {
    p("close", timer);
  });
});
uv.run();


print("\nTest two concurrent timers");
var timer1 = new uv.Timer();
var timer2 = new uv.Timer();
timer1.start(1000, 0, function () {
  p("timeout", timer1);
  timer1.close();
  timer2.close();
});
timer2.start(100, 200, function () {
  p("interval", timer2);
});
uv.run();

print("\nTest shrinking interval");
timer = new uv.Timer();
timer.start(10, 0, function () {
  var repeat = timer.getRepeat();
  p("interval", timer, repeat);
  if (repeat === 0) {
    timer.setRepeat(32);
    timer.again();
  }
  else if (repeat == 2) {
    timer.stop();
    timer.close();
  }
  else {
    timer.setRepeat(repeat / 2);
  }
});
uv.run();

print("\nTesting scandir at root");
p(nucleus.scandir('.', p));

print("\nTesting scandir at deps");
p(nucleus.scandir('deps', p));

print("\nTest various buffer types");
p(Duktape.Buffer("test"));
p(new Duktape.Buffer("test"));
p(new ArrayBuffer("test"));
