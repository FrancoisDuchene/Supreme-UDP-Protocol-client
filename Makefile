OPT = -Wall -W -Werror
CFLAGS = -std=gnu99 -g -lz
HFILES = headers.h packet_interface.h
COMMONCFILES = packet_implem.c

.SILENT:

all: clean packet end

packet:
	gcc $(COMMONCFILES) $(HFILES) -o packet $(CFLAGS) $(OPT)

sender:
	echo "TODO"

test:
	gcc test/format_tests.c -o tests $(CFLAGS) -lcunit $(OPT)

clean:
	rm -f packet

end:
	echo "MakeFile completed succefully"

.PHONY: all tests
