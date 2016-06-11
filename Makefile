test-duktape: duktape
	tools/test.py --mode=duktape

duktape:
	make -C implementations/duktape nucleus
