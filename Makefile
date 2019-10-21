OPT = -g -lz
CFLAGS += -std=c99 # Define which version of the C standard to use
CFLAGS += -Wall # Enable the 'all' set of warnings
CFLAGS += -Werror # Treat all warnings as error
CFLAGS += -Wshadow # Warn when shadowing variables
CFLAGS += -Wextra # Enable additional warnings
CFLAGS += -O2 -D_FORTIFY_SOURCE=2 # Add canary code, i.e. detect buffer overflows
CFLAGS += -fstack-protector-all # Add canary code to detect stack smashing
CFLAGS += -D_POSIX_C_SOURCE=201112L -D_XOPEN_SOURCE # feature_test_macros for getpot and getaddrinfo
HFILES = packet_interface.h sender.h general.h init_connexion.h read_write_loop_final.h window.h pkt_builder.h linked_list.h
COMMONCFILES = packet_implem.c sender.c init_connexion.c read_write_loop_final.c utils.c window.c pkt_builder.c linked_listcTESTFILES = test/test_main.c test/test.h test/format_tests.c

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
