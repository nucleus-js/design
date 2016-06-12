function assert(thing, message) {
  if (thing) return

  throw new Error(message)
}

assert(nucleus.readfile('../fixtures/a.js') === "return 'a'\n", 'a.js must return it\'s raw source')

assert(nucleus.readfile('../fixtures/DOES-NOT-EXIST') === null, 'Non-existant file should return null')
