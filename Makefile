OPT = -Wall -W -Werror
CFLAGS = -std=gnu99 -g -lz
HFILES = headers.h packet_interface.h
COMMONCFILES = packet_implem.c
TESTFILES = test/test_main.c test/test.h test/format_tests.c

.SILENT:

all: clean packet end

packet:
	gcc $(COMMONCFILES) $(HFILES) -o packet $(CFLAGS) $(OPT)

sender:
	echo "TODO"

tests:
	rm -f test_launch
	gcc $(TESTFILES) $(COMMONCFILES) $(HFILES) -o test_launch -lcunit $(CFLAGS) $(OPT)

clean:
	rm -f packet

end:
	echo "MakeFile completed succefully"

.PHONY: all tests
