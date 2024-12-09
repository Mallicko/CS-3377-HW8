# Compiler and flags
CC = gcc
CFLAGS = -Wall

# Targets
build:
	$(CC) -o echo_s echo_s.c
	$(CC) -o echo_c echo_c.c
	$(CC) -o log_s log_s.c

run_echo_s:
	./echo_s 8080
run_echo_c:
	./echo_c 127.0.0.1 8080 tcp
run_log_s:
	./log_s

clean:
	rm -f echo_s echo_c log_s
