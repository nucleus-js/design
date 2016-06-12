var assert = nucleus.dofile("../common.js").assert

var a = nucleus.compile("return 'a'", 'a')
assert(a() === 'a', "compile should return 'a'")

var err = nucleus.compile("return new Error('oops')", 'test-nucleus-compile.js')
print(/\(test-nucleus-compile\.js:1\)/.test(err().stack), "nucleus must contain 'test-nucleus-compile.js'")
