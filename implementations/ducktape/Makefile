# CC= musl-gcc -static -Os

nucleus: main.o miniz.o duktape.o
	${CC} $^ -lm  -o $@

main.o: main.c
	${CC} -Wall -Wextra -pedantic -c $< -o $@

duktape.o: duktape-releases/src/duktape.c duktape-releases/src/duktape.h
	${CC} -Wall -Wextra -pedantic -c $< -o $@

miniz.o: ../../deps/miniz.c
	${CC} -std=c99 -c $< -o $@

clean:
	rm -f nucleus *.o
