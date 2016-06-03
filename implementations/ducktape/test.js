var p = nucleus.dofile("deps/utils.js").prettyPrint;
p(nucleus);
p(nucleus.readfile('main.c'));

var fn = nucleus.compile('return 2 - 1;', 'myfunc');
p(fn);
p(fn());
p(nucleus.scandir('.', p));
