APPS=\
  echo-server

all: ${APPS}

%: ../nucleus %.zip
	cat $^ > $@
	chmod +x $@

%.zip: %-src %-src/*
	cd $<; zip -r ../$@ *.js; cd -

../nucleus:
	${MAKE} -C ..

clean:
	rm -f *.zip ${APPS}
