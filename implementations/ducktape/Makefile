CC= musl-gcc -static -Os

nucleus: main.c miniz.o ../../deps/miniz.c duktape.o duktape-releases/src/duktape.h
	${CC} -Wall -Wextra -pedantic -std=c11 $< duktape.o miniz.o -o $@

duktape.o: duktape-releases/src/duktape.c duktape-releases/src/duktape.h
	${CC} -Wall -Wextra -pedantic -std=c99 -c $< -o $@

miniz.o: ../../deps/miniz.c
	${CC} -std=c99 -c $< -o $@

clean:
	rm -f nucleus *.o
