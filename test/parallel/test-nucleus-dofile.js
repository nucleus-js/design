var assert = nucleus.dofile("../common.js").assert

assert(nucleus.dofile("../fixtures/a.js") === 'a', "a.js should expose 'a'")
