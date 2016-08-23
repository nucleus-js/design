var assert = nucleus.dofile("./common/assert.js")

// get non-existing
assert(nucleus.getenv('__NUCLEUS_TEST') === null, 'nonexisting env var should be `null`')

// set new
assert(nucleus.setenv('__NUCLEUS_TEST', 'nice day @nodeconf') === undefined, 'setenv should return `undefined`')

// get newly set
assert(nucleus.getenv('__NUCLEUS_TEST') === 'nice day @nodeconf', 'getenv should return value')

// newly set in envkeys
assert(nucleus.envkeys().indexOf('__NUCLEUS_TEST') !== -1, 'env variable should be set in envkeys')

// re-set
assert(nucleus.setenv('__NUCLEUS_TEST', 'hi there') === undefined, 'setenv should return `undefined`')

// get re-set
assert(nucleus.getenv('__NUCLEUS_TEST') === 'hi there', 'get existing key should return `hi there`')

// re-set in envkeys
assert(nucleus.envkeys().indexOf('__NUCLEUS_TEST') !== -1, 'env variable should be set in envkeys')

// unset
assert(nucleus.unsetenv('__NUCLEUS_TEST') === undefined, 'setenv should return `undefined`')

// get now non-existing
assert(nucleus.getenv('__NUCLEUS_TEST') === null, 'nonexisting env var should be `null`')

// no longer in envkeys
assert(nucleus.envkeys().indexOf('__NUCLEUS_TEST') === -1, 'env variable should be set in envkeys')
