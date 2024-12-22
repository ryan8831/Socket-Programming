/*******************************************************/
/* File  : udpclient.c (BF_INET)                       */
/* Usage : udpclient server port                       */
/* Desc. : Send a message to UDP server                */
/*         Recv an upper-caseing ECHO from UDP server  */
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
void error(char *);

int main(int argc, char **argv)
{
   int sockfd, length, retval;
   struct sockaddr_in srvaddr, peeraddr;
   
   char buffer[BUFFER_SIZE], echobuffer[BUFFER_SIZE];
   
   char peerip[20];
   int peerport;
     
   if (argc != 3) { 
   	fprintf(stderr, "Usage: %s server port\n", argv[0]); // 如果命令列參數數量不正確，輸出使用說明
        exit(1); // 結束程式
   }
   
   sockfd = socket(PF_INET, SOCK_DGRAM, 0); // 創建一個UDP Socket
   
   if (sockfd < 0) {
   	fprintf(stderr, "Datagram socket 建立失敗\n"); // 如果建立Socket失敗，輸出錯誤訊息並結束程式
   	exit(1);
   }

   bzero(&srvaddr, sizeof(srvaddr)); // 清空伺服器位址結構

   srvaddr.sin_family = PF_INET;
   if (inet_aton(argv[1], &srvaddr.sin_addr) == 0) {
   	fprintf(stderr, "UDP Server IP 格式錯誤 : %s\n", argv[1]); // 如果IP位址格式錯誤，輸出錯誤訊息並結束程式
   	exit(1);
   }   
   srvaddr.sin_port = htons(atoi(argv[2])); // 設定伺服器通訊埠
   length = sizeof(srvaddr);
   
   for(;;) { // 無限迴圈
     fprintf(stdout, "請輸入UDP 訊息 (Enter鍵結束!) (輸入exit終止程式！): "); // 提示用戶輸入UDP訊息
     bzero(&buffer, BUFFER_SIZE);
     fgets(buffer, BUFFER_SIZE, stdin);
     buffer[strlen(buffer) - 1] = '\0'; // 移除換行符
     if (strcmp(buffer, "exit") == 0)
       break; // 如果輸入 "exit" 則結束程式
     
     retval = sendto(sockfd, buffer, strlen(buffer), 0,
     	(struct sockaddr *)&srvaddr, length); // 傳送UDP訊息到伺服器
     if (retval < 0) {
     	fprintf(stderr, "sendto function 發生錯誤,無法送出 UDP 訊息!!\n"); 
     	// 如果傳送訊息失敗，輸出錯誤訊息並結束程式
     	exit(1);
     }
     fprintf(stdout, "送出 UDP 訊息: %s [送至 %s:%d]\n", buffer, argv[1], atoi(argv[2])); 
     // 輸出已傳送的UDP訊息及目的IP和通訊埠
   
     bzero(&echobuffer, BUFFER_SIZE); // 清空回應緩衝區
     retval = recvfrom(sockfd, echobuffer, BUFFER_SIZE, 0,
     	(struct sockaddr *)&peeraddr, &length); // 接收從伺服器回傳的UDP訊息
     if (retval < 0) {
     	fprintf(stderr, "recvfrom function 發生錯誤,無發接收 UDP 訊息!!\n"); // 如果接收訊息失敗，輸出錯誤訊息並結束程式
     	exit(1);
     }
   
     bzero(&peerip, 20);
     strcpy(peerip, inet_ntoa(peeraddr.sin_addr)); // 獲取來源IP位址
     peerport = ntohs(peeraddr.sin_port); 
     fprintf(stdout, "收到 UDP 訊息: %s [來至 %s:%d]\n", echobuffer, peerip, peerport); // 輸出已接收的UDP訊息及來源IP和通訊埠
   }
   close(sockfd); // 關閉Socket
   exit(0); // 正常結束程式
}
