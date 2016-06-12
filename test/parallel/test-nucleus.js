// print("\nTesting dofile by loading pretty-printer library from another file");
// var p = nucleus.dofile("deps/utils.js").prettyPrint;
// p(p);

var assert = nucleus.dofile("../common.js").assert

// Make sure nucleus is an object
assert(nucleus && typeof nucleus === 'object', '`nucleus` is not an Object')

// nucleus.base .../nucleus/tests
assert(typeof nucleus.base === 'string', '`nucleus.base` must be a String')

// nucleus.cmd ./nucleus
assert(typeof nucleus.cmd === 'string', '`nucleus.cmd` must be a String')

// nucleus.args [ 'a', 'b', 'c', ... ]
assert(Array.isArray(nucleus.args), '`nucleus.args` must be an Array')

// nucleus.rawArgs
assert(Array.isArray(nucleus.rawArgs), '`nucleus.rawArgs` must be an Array')

// nucleus.cmd should be the same as rawArgs[0]
assert(nucleus.cmd === nucleus.rawArgs[0], '`nucleus.cmd` must be the same as rawArgs[0]')

// nucleus.engine
assert(typeof nucleus.engine === 'string', '`nucleus.engine` must be a String')

// nucleus.versions
assert(nucleus.versions && typeof nucleus.versions === 'object', '`nucleus.versions` must be an Object')

// nucleus.envkeys
assert(typeof nucleus.envkeys === 'function', '`nucleus.envkeys` must be a Function')
assert(Array.isArray(nucleus.envkeys()), '`nucleus.envKeys` must return an Array')

// nucleus.uv
assert(nucleus.uv && typeof nucleus.uv === 'object', '`nucleus.uv` is not an Object')
