OPT = -g -lz
CFLAGS += -std=c99 # Define which version of the C standard to use
CFLAGS += -Wall # Enable the 'all' set of warnings
CFLAGS += -Werror # Treat all warnings as error
CFLAGS += -Wshadow # Warn when shadowing variables
#CFLAGS += -Wextra # Enable additional warnings
CFLAGS += -O2 -D_FORTIFY_SOURCE=2 # Add canary code, i.e. detect buffer overflows
CFLAGS += -fstack-protector-all # Add canary code to detect stack smashing
CFLAGS += -D_POSIX_C_SOURCE=201112L -D_XOPEN_SOURCE # feature_test_macros for getpot and getaddrinfo
HFILES = src/packet_interface.h  src/general.h src/init_connexion.h src/read_write_loop_final.h src/window.h src/pkt_builder.h src/linked_list.h
COMMONCFILES = src/packet_implem.c src/init_connexion.c src/read_write_loop_final.c src/utils.c src/window.c src/pkt_builder.c src/linked_list.c 
TESTFILES = tests/test_main.c tests/test.h tests/format_tests.c tests/linkedlist_tests.c

.SILENT:

all: clean sender end

sender:
	gcc $(COMMONCFILES) src/sender.c $(HFILES) src/sender.h -o sender $(CFLAGS) $(OPT)

tests:
	rm -f test_launch
	gcc $(TESTFILES) $(COMMONCFILES) $(HFILES) -o test_launch -lcunit $(CFLAGS) $(OPT)

checks:
	cppcheck $(COMMONCFILES) src/sender.c $(HFILES) src/sender.h

clean:
	#rm -f packet
	rm -f sender

end:
	echo "MakeFile completed succefully"

.PHONY: all tests
