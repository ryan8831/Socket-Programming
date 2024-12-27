CC = gcc

CFLAGS = -Wall

all:    sctp_client sctp_server

sctp_client: sctp_client.c
        $(CC) $(CFLAGS) -o $@ sctp_client.c -L/usr/local/lib -lsctp

sctp_server: sctp_server.c
        $(CC) $(CFLAGS) -o $@ sctp_server.c -L/usr/local/lib -lsctp

===============
執行結果

[root@localhost sctp]# ./sctp_server  1234  &

[root@localhost sctp]# ./sctp_client  127.0.0.1 1234
Input:HelloBear!
(UPPER) HELLOBEAR!
(LOWER) hellobear!
(ECHO) HelloBear!