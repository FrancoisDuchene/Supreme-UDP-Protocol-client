OPT = -Wall -W -Werror
CFLAGS = -std=gnu99 -g -lz
HFILES = headers.h packet_interface.h
COMMONCFILES = packet_implem.c

.SILENT:

all: packet end

packet:
	gcc $(COMMONCFILES) $(HFILES) -o packet $(CFLAGS) $(OPT)

sender:
	echo "TODO"

end:
	echo "MakeFile completed succefully"

.PHONY: all tests
