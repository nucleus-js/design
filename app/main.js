print("\nTesting dofile by loading pretty-printer library from another file");
var p = nucleus.dofile("deps/utils.js").prettyPrint;
p(p);

print("\nTesting pretty printer by dumping nucleus");
p(nucleus);

print("\nTesting scandir at root");
p(nucleus.scandir('.', p));

print("\nTesting scandir at deps");
p(nucleus.scandir('deps', p));

print("\nTest various buffer types");
p(Duktape.Buffer("test"));
p(new Duktape.Buffer("test"));
p(new ArrayBuffer("test"));
