var assert = nucleus.dofile("./common/assert.js")

assert(nucleus.dofile("../fixtures/a.js") === 'a', "a.js should expose 'a'")
