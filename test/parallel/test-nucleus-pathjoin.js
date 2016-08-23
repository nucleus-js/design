var assert = nucleus.dofile("./common/assert.js")

assert(nucleus.pathjoin('a', 'b', 'c') === 'a/b/c', 'pathjoin must work')
