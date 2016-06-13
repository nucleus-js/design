LIBUV= ../../deps/libuv

# Make sure to `make distclean` before building when changing CC.
# Default build is debug mode.
CC= cc -g
# Uncomment the following to make a small binary
# CC= cc -Os
# Uncomment the following to make a static musl binary on linux
# CC= musl-gcc -Os -static

nucleus: main.o miniz.o duktape.o duv/duv.a ${LIBUV}/.libs/libuv.a env.o rust_path/target/release/libc_path.a
	${CC} $^ -lm -lpthread -o $@ rust_path/target/release/libc_path.a -ldl

rust_path/target/release/libc_path.a: rust_path/src/lib.rs rust_path/src/helpers.rs
	cd rust_path && cargo build --release

install: nucleus
	install nucleus /usr/local/bin/

test:
	make -C ../.. test-duktape

test-manual: test-dir test-zip test-app test-app-tiny

test-dir: nucleus
	./nucleus ../../test/manual -- 1 2 3

test-zip: nucleus app.zip
	./nucleus app.zip -- 4 5 6

test-app: app
	./app 7 8 9

test-app-tiny: app-tiny
	./app-tiny 10 11 12

app: app.zip nucleus
	cat nucleus app.zip > app
	chmod +x app

app-tiny: app.zip prefix
	cat prefix app.zip > app-tiny
	chmod +x app-tiny

prefix: nucleus
	echo "#!$(shell pwd)/nucleus --" > prefix

app.zip: ../../test/manual/* ../../test/manual/deps/*
	rm -f app.zip
	cd ../../test/manual; zip -9 -r ../../implementations/duktape/app.zip .; cd -

env.o: env.c env.h
	${CC} -std=gnu99 -Wall -Wextra -pedantic -Werror -c $< -o $@

main.o: main.c *.h
	${CC} -std=gnu99 -Wall -Wextra -pedantic -Werror -c $< -o $@


${LIBUV}/.libs/libuv.a: ${LIBUV}/Makefile
	${MAKE} -C ${LIBUV}

${LIBUV}/Makefile: ${LIBUV}/configure
	cd ${LIBUV}; ./configure; cd -

${LIBUV}/configure: ${LIBUV}/autogen.sh
	cd ${LIBUV}; ./autogen.sh; cd -

duv/duv.a: duv/*.c duv/*.h
	${MAKE} -C duv

duktape.o: duktape-releases/src/duktape.c duktape-releases/src/duktape.h
	${CC} -std=c99 -Wall -Wextra -pedantic -c $< -o $@

miniz.o: ../../deps/miniz.c
	${CC} -std=gnu99 -c $< -o $@

clean:
	rm -rf nucleus *.o app.zip app prefix app-tiny rust_path/target
	${MAKE} -C duv clean

distclean: clean
	cd ${LIBUV}; git clean -xdf; cd -
