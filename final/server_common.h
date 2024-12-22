#ifndef _SERVER_COMMON_H
#define _SERVER_COMMON_H

#define CLIENT_NUM     2      // 房間容量數
#define MAX_CLIENT_NUM 64
#define MIN_CLIENT_NUM 2
#define PLAYER_NUM     2      //玩家數量
#define MAX_PLAYER_NUM 26
#define MIN_PLAYER_NUM 2
extern int create_listen(int port);

extern int deal_player_action(int client_sock,int* retx,int* rety,char* retmsg);

extern int checkWin(int x,int y,char gomoku[][N],char flagWin);

extern int clients[CLIENT_NUM];  // 允許CLIENT_NUM個客戶端在線
extern int player_on_sock[26];
extern fd_set liveset, tempset, revset, t_revset; // 客戶端 sockets 

#endif

