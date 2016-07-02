test-seaduk: seaduk
	tools/test.py --mode=seaduk

seaduk:
	${MAKE} -C implementations/seaduk target/nucleus

distclean:
	${MAKE} -C implementations/seaduk distclean
