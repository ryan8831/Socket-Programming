#ifndef COMMON_H
#define COMMON_H

#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <strings.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <time.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>

#define N 10   //棋盤大小
#define IP "127.0.0.1"
#define PORT 10001

typedef struct sockaddr* pSA;
typedef struct sockaddr_in SA_in;
typedef struct Message_pakage //發送訊息
{
	int type;//訊息類型：0表示可以下棋，1表示遊戲狀態更新，2表示提示訊息，3表示遊戲结束，-1表示錯誤訊息
	int isstart;//判斷遊戲開始
	int sock;//目標sock
	int x,y; //座標
	char player_tpye;//玩家類型，用於棋盤標記
	char message[256];//訊息内容
} MSGPAK;
extern void send_pak(int type, int sock, int x, int y,
                     char player_tpye, char* message);
extern int isstart;//判斷遊戲開始
extern char table[N][N];

#endif

