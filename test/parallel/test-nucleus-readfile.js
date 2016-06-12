var assert = nucleus.dofile("../common.js").assert

assert(nucleus.readfile('../fixtures/a.js') === "return 'a'\n", 'a.js must return it\'s raw source')
