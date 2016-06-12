var assert = nucleus.dofile("./common/assert.js")

assert(nucleus.readfile('../fixtures/a.js') === "return 'a'\n", 'a.js must return it\'s raw source')
