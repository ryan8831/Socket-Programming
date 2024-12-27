/*******************************************************/
/* File  : udpserver.c (BF_INET)                       */
/* Usage : udpserver port                              */
/* Desc. : Recv a message from UDP client              */
/*         sned an upper-caseing ECHO to UDP Client    */
/*******************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char **argv)
{
   int sock, length, fromlen, retval;
   struct sockaddr_in srvaddr, fromaddr ;
   
   char fromip[40] ;
   int fromport ;
   
   char buffer[BUFFER_SIZE];
   char echobuf[BUFFER_SIZE] ;

   if (argc < 2) {
      fprintf(stderr, "Usage: %s port\n", argv[1]); // 如果命令列參數數量不足，輸出使用說明
      exit(1); // 結束程式
   }
   
   sock = socket(PF_INET, SOCK_DGRAM, 0); // 創建一個UDP Socket
   if (sock < 0) {
      fprintf(stderr, "Datagram Socket 開啟失敗!!\n"); // 如果建立Socket失敗，輸出錯誤訊息並結束程式
      exit(1);
   }
   length = sizeof(srvaddr);
   bzero(&srvaddr, length);
   srvaddr.sin_family = PF_INET;
   srvaddr.sin_addr.s_addr = INADDR_ANY;
   srvaddr.sin_port = htons(atoi(argv[1])); // 綁定Socket到指定通訊埠
   if (bind(sock, (struct sockaddr *)&srvaddr, length) < 0) {
       fprintf(stderr, "Socket Bind 錯誤,無法連結 Port : %d", atoi(argv[1])); // 如果綁定Socket失敗，輸出錯誤訊息並結束程式
       exit(1);
   }
   fromlen = sizeof((struct sockaddr_in)fromaddr);
   while (1) {
       fprintf(stdout, "等待 UDP 訊息到來...\n"); // 輸出等待UDP訊息的訊息
       bzero(&buffer, BUFFER_SIZE); // 清空接收緩衝區
       bzero(&fromaddr, fromlen); // 清空來源位址結構
       retval = recvfrom(sock, buffer, BUFFER_SIZE, 0,
            (struct sockaddr *)&fromaddr, &fromlen); // 接收UDP訊息
       if (retval < 0) {
           fprintf(stderr, "recvfrom function 發生錯誤,無法接收UDP 訊息!!\n"); // 如果接收訊息失敗，輸出錯誤訊息並結束程式
           exit(1);
       }
       
       bzero(&fromip, 20); // 清空來源IP字串
       strcpy(fromip, inet_ntoa(fromaddr.sin_addr)); // 獲取來源IP位址
       fromport = ntohs(fromaddr.sin_port); // 獲取來源通訊埠
       fprintf(stdout, "接收 UDP 訊息: %s [來至 %s:%d]\n", buffer, fromip, fromport); // 輸出接收到的UDP訊息及來源IP和通訊埠

       bzero(&echobuf, BUFFER_SIZE); // 清空回應緩衝區
       strcpy(echobuf, buffer); // 複製接收到的訊息到 echobuf
       retval = sendto(sock, echobuf, BUFFER_SIZE,
                  0, (struct sockaddr *)&fromaddr, fromlen); // 傳送回應訊息
       if (retval < 0) {
           fprintf(stderr, "Sendto function 發生錯誤,無法傳送UDP 訊習!!\n"); // 如果傳送回應訊息失敗，輸出錯誤訊息並結束程式
           exit(1);
       }
       fprintf(stdout, "回應 UDP 訊息: %s [送至 %s:%d]\n", echobuf, fromip, fromport); // 輸出已回應的UDP訊息及目的IP和通訊埠
       
   }
   close(sock) ; // 關閉Socket
   }
