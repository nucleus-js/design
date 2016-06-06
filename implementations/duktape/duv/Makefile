LIBS=\
	loop.o\
	handle.o\
	timer.o\
	prepare.o\
	check.o\
	idle.o\
	async.o\
	poll.o\
	signal.o\
	process.o\
	stream.o\
	tcp.o\
	pipe.o\
	tty.o\
	udp.o\
	fs_event.o\
	fs_poll.o\
	fs.o\
	dns.o\
	misc.o\
	duv.o\
	utils.o\
	callbacks.o\
	dschema.o

duv.a: ${LIBS}
	${AR} cr $@ ${LIBS}

%.o: %.c %.h
	${CC} -std=gnu99 -Wall -Wextra -pedantic -Werror -c $< -o $@

clean:
	rm -f *.o *.a
