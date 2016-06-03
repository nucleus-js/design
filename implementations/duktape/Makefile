test: test-dir test-zip test-app test-app-tiny

test-dir: nucleus
	./nucleus ../../app -- 1 2 3

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

app.zip: ../../app* ../../app/deps/*
	rm -f app.zip
	cd ../../app; zip -9 -r ../implementations/duktape/app.zip .; cd -

static: main.c
	musl-gcc -static -Os ../../deps/miniz.c main.c duktape-releases/src/duktape.c -o nucleus

nucleus: main.o miniz.o duktape.o
	${CC} $^ -lm  -o $@

main.o: main.c
	${CC} -std=c99 -Wall -Wextra -pedantic -c $< -o $@

duktape.o: duktape-releases/src/duktape.c duktape-releases/src/duktape.h
	${CC} -std=c99 -Wall -Wextra -pedantic -c $< -o $@

miniz.o: ../../deps/miniz.c
	${CC} -std=gnu99 -c $< -o $@

clean:
	rm -f nucleus *.o app.zip app prefix app-tiny
