function assert(thing, message) {
  if (thing) return

  throw new Error(message)
}

assert(nucleus.pathjoin('a', 'b', 'c') === 'a/b/c', 'pathjoin must work')
