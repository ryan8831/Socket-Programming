 /* tcpserver.c */
 
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h> /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <ctype.h> /* toupper() */
#include <string.h>
#include <unistd.h>

#define PORT 10000           /* server port value */

int main()
{
  int accept_sock;
  int client_sock;
  struct sockaddr_in serv_addr;
  char ch;
  char arr[100]={0};
  int index;
	
  /* create INTERNET,TCP socket */
  accept_sock = socket(PF_INET, SOCK_STREAM, 0);
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons(PORT);      /* specific port */

  /* bind protocol to socket */
  if(bind(accept_sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr))< 0)
  {
    perror("bind");
    exit(1);
  }
        
  listen(accept_sock,5);  //listen

  for(;;)
  {
    client_sock = accept(accept_sock, 0, 0);  //accept the connection
    
    memset(arr, 0, sizeof(arr));  //清除字串
    index=0;
          
    for(;;)
    {
      if(recv(client_sock, &ch, sizeof ch, 0) <= 0)  //是否接收到字串
        break;
              
      arr[index]=ch;  //儲存接收到的字串             
      index++;
             
      send(client_sock, &ch, sizeof ch, 0);  //寄送字串給client
            
      if(ch == '\0')  //end of string
      {         
        printf("From client:%s\n",arr);  //顯示client送來的字串
        break;
      }
              
    }
          
    close(client_sock);  //關閉socket
  }
  
  return 0;
  
}
