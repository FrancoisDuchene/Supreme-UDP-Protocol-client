OPT = -Wall -W -Werror
CFLAGS = -std=gnu99 -g -lz
HFILES = headers.h packet_interface.h sender.h
COMMONCFILES = packet_implem.c sender.c
TESTFILES = test/test_main.c test/test.h test/format_tests.c

.SILENT:

all: clean sender end

packet:
	gcc $(COMMONCFILES) $(HFILES) -o packet $(CFLAGS) $(OPT)

sender:
	gcc $(COMMONCFILES) $(HFILES) -o sender $(CFLAGS) $(OPT)

tests:
	rm -f test_launch
	gcc $(TESTFILES) $(COMMONCFILES) $(HFILES) -o test_launch -lcunit $(CFLAGS) $(OPT)

clean:
	#rm -f packet
	rm -f sender

end:
	echo "MakeFile completed succefully"

.PHONY: all tests
