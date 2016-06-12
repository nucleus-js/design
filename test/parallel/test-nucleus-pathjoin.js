var assert = nucleus.dofile("../common.js").assert

assert(nucleus.pathjoin('a', 'b', 'c') === 'a/b/c', 'pathjoin must work')
