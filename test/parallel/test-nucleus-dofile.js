
function assert(thing, message) {
  if (thing) return

  throw new Error(message)
}

assert(nucleus.dofile("../fixtures/a.js") === 'a', "a.js should expose 'a'")
