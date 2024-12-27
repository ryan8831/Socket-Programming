/* tcpclient.c */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>  /* basic system data types */
#include <sys/socket.h> /* basic socket definitions */
#include <netinet/in.h> /* sockaddr_in{} and other Internet defns */
#include <arpa/inet.h> /*for inet_addr()  */
#include <unistd.h>

#define PORT 10000           /* server port value */

int main()
{
  int connect_sock;
  char input[100], output[100], *s = output;
  struct sockaddr_in serv_addr;
  int n;
  
  /* create INTERNET,TCP socket */
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = htons(PORT);
  serv_addr.sin_addr.s_addr = inet_addr("140.123.106.146");
  
  connect_sock = socket(PF_INET, SOCK_STREAM, 0);  //建立client端socket
  
  do
  {
    connect_sock = socket(PF_INET, SOCK_STREAM, 0);  //重新建立建立client端socket
  
    if (connect(connect_sock, (struct sockaddr *) &serv_addr, sizeof serv_addr) <0)  //連接server
    {  
      perror("connect");
      exit(1); 
    }
  
    printf("Enter the string:");  //輸入要傳送的字串
    scanf("%s", input);
    
    send(connect_sock, input, strlen(input) + 1 ,0);  //寄送剛才輸入的字串including ending '\0' 到server
    
    while((n = recv(connect_sock, s, sizeof *s, 0)) > 0) {  //接收字串
      s += n;                                         // move pointer for recv()
    }
    printf("From Server:%s\n", output);  //顯示server送來的字串
    s=output;
    
    close(connect_sock);  //關閉socket
  }
  while(*input!='0');  //輸入0中止程式
  
  return 0;
  
}
