
function assert(thing, message) {
  if (thing) return

  throw new Error(message)
}

// get non-existing
assert(nucleus.getenv('__NUCLEUS_TEST') === null)

// set new
assert(nucleus.setenv('__NUCLEUS_TEST', 'nice day @nodeconf') === undefined)

// get newly set
assert(nucleus.getenv('__NUCLEUS_TEST') === 'nice day @nodeconf')

// newly set in envkeys
assert(nucleus.envkeys().indexOf('__NUCLEUS_TEST') !== -1)

// re-set
assert(nucleus.setenv('__NUCLEUS_TEST', 'hi there') === undefined)

// get re-set
assert(nucleus.getenv('__NUCLEUS_TEST') === 'hi there')

// re-set in envkeys
assert(nucleus.envkeys().indexOf('__NUCLEUS_TEST') !== -1)

// unset
assert(nucleus.unsetenv('__NUCLEUS_TEST') === undefined)

// get now non-existing
assert(nucleus.getenv('__NUCLEUS_TEST') === null)

// no longer in envkeys
assert(nucleus.envkeys().indexOf('__NUCLEUS_TEST') === -1)
