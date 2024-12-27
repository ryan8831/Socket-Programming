#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <arpa/inet.h> /* inet_addr */
#include <ctype.h> /* toupper() */
#include <unistd.h> /* close () */

#define BUFFER_SIZE 1024

#define STREAM_UPPER 0
#define STREAM_LOWER 1
#define STREAM_ECHO  2

int main(int argc, char *argv[])
{
  int sockfd;
  struct sockaddr_in srvaddr;
  struct sctp_initmsg initmsg;
  struct sctp_sndrcvinfo sndrcvinfo;
  struct sctp_event_subscribe events;
  int i;
  char in[BUFFER_SIZE], out[BUFFER_SIZE];

  if (argc != 3) {
     fprintf(stderr, "Usage: %s server port\n", argv[0]);
     exit(1);
  }

  /* Create SCTP TCP-Style Socket */
  sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP );

  /* Specify that a maximum of 3 streams will be available per socket */
  memset( &initmsg, 0, sizeof(initmsg) );
  initmsg.sinit_num_ostreams = 3;
  initmsg.sinit_max_instreams = 3;
  initmsg.sinit_max_attempts = 4;
  setsockopt( sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof initmsg);


  /* Specify the peer endpoint to which we'll connect */
  srvaddr.sin_family = AF_INET;
  srvaddr.sin_addr.s_addr = inet_addr(argv[1]);
  srvaddr.sin_port=htons(atoi(argv[2]));

  /* Connect to the server */
  if( connect( sockfd, (struct sockaddr *)&srvaddr, sizeof(srvaddr) )<0) {
    perror("connect");
    exit(1);
  }

  /* Enable receipt of SCTP Snd/Rcv Data via sctp_recvmsg */
  memset(&events, 0, sizeof(events));
  events.sctp_data_io_event = 1;
  setsockopt(sockfd, SOL_SCTP, SCTP_EVENTS, &events, sizeof(events) );

  /* user input */
  printf("Input:");
  scanf("%s", in);

  sctp_sendmsg(sockfd, in, strlen(in) + 1, NULL, 0, 0, 0, 0, 0, 0);
for (i = 0 ; i < 3 ; i++) {
    if(sctp_recvmsg( sockfd, out, sizeof out, NULL, 0, &sndrcvinfo, NULL)>0) {
      switch(sndrcvinfo.sinfo_stream) {
        case STREAM_UPPER:
          printf("(UPPER) %s\n", out);
          break;
        case STREAM_LOWER:
          printf("(LOWER) %s\n", out);
          break;
        case STREAM_ECHO:
          printf("(ECHO) %s\n", out);
          break;
        default:
          break;
      }
    }
  }
  /* Close our socket and exit */
  close(sockfd);
  return 0;
}