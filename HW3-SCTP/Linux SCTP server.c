#include <stdio.h> /* fprintf() */
#include <stdlib.h> /* exit () */
#include <string.h>  /* memset() */
#include <ctype.h> /* toupper() */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> /* INADDR_ANY */
#include <netinet/sctp.h>
#include <unistd.h>     /* close */

#define BUFFER_SIZE 1024

#define STREAM_UPPER 0
#define STREAM_LOWER 1
#define STREAM_ECHO  2

int main(int argc, char *argv[])
{
   int sockfd, clientfd;
   struct sockaddr_in srvaddr;
   struct sctp_initmsg initmsg;

   int i;
   char in[BUFFER_SIZE], out1[BUFFER_SIZE], out2[BUFFER_SIZE];

   if (argc < 2) {
      fprintf(stderr, "Usage: %s port\n", argv[0]);
      exit(1);
   }

   /* Create SCTP TCP-Style Socket */
   sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_SCTP );
   srvaddr.sin_family = AF_INET;
   srvaddr.sin_addr.s_addr = INADDR_ANY;
   srvaddr.sin_port=htons(atoi(argv[1]));

   if (bind(sockfd, (struct sockaddr *)&srvaddr, sizeof srvaddr)<0) {
       perror("bind");
       exit(1) ;
   }
   /* Specify that a maximum of 5 streams will be available per socket */
  memset( &initmsg, 0, sizeof(initmsg) );
  initmsg.sinit_num_ostreams = 5;
  initmsg.sinit_max_instreams = 3;
  initmsg.sinit_max_attempts = 4;
  setsockopt( sockfd, IPPROTO_SCTP, SCTP_INITMSG, &initmsg, sizeof initmsg);

  /* Place the server socket into the listening state */
  listen( sockfd, 5 );

  while (1) {
    clientfd = accept( sockfd, NULL, NULL);

    /* New client socket has connected */
    sctp_recvmsg(clientfd, in, sizeof in, NULL, NULL, NULL, NULL);

    for(i = 0; i <= strlen(in); i++) {
      out1[i] = toupper(in[i]);
      out2[i] = tolower(in[i]);
    }

    /* Send upper stream on stream 0 */
    sctp_sendmsg( clientfd, out1, i, NULL, 0, 0, 0, STREAM_UPPER, 0, 0 );

    /* Send lower stream on stream 1 */
    sctp_sendmsg( clientfd, out2, i, NULL, 0, 0, 0, STREAM_LOWER, 0, 0 );

    /* Send lower stream on stream 2 */
    sctp_sendmsg( clientfd, in, i, NULL, 0, 0, 0, STREAM_ECHO, 0, 0 );

    close(clientfd) ;
  }
}