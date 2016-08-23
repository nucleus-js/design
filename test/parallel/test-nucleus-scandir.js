var assert = nucleus.dofile("./common/assert.js")

var directory = nucleus.scandir('../fixtures',  dirOnEntry)
assert(directory === true, 'scandir must see `fixtures` as a directory')

// FIXME(Fishrock123): hopefully this gets called?
// Not sure how to ensure it without having an exit catch handler...
function dirOnEntry(name, type) {
  assert(typeof name === 'string', 'name must be a string: ' + name)
  assert(/^(directory|file)$/.test(type), "type must be 'directory' or 'file', was: " + type)
}

try {
  var threw = true;
  nucleus.scandir('../fixtures/a.js', function(){})
  threw = false;
} catch (e) {
} finally {
  assert(threw, 'scandir on a file must throw')
}

var invalid = nucleus.scandir('../fixtures/DOES-NOT-EXIST',  failOnCall)
assert(invalid === null, 'scandir must see `fixtures/DOES-NOT-EXIST` as ENOENT (null)')

function failOnCall() {
  throw new Error('Should not be called!')
}
