test-seaduk: seaduk
	tools/test.py --mode=seaduk

seaduk:
	make -C implementations/seaduk target/nucleus
