
function assert(thing, message) {
  if (thing) return

  throw new Error(message)
}

try {
  var threw = true
  nucleus.exit()
  threw = false
} catch (e) {
} finally {
  assert(threw, 'exit() without an argument should throw')
}

try {
  var threw = true
  nucleus.exit('1')
  threw = false
} catch (e) {
} finally {
  assert(threw, 'exit() called with a non-numeric value should throw')
}
