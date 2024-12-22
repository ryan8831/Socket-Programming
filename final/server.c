#include "common.h"
#include "server_common.h"
#include "handle_sigpipe.h"
#include <semaphore.h>

MSGPAK pak;

sem_t sem;
struct timeval timeout = {3, 0};
int actual_player,client_num;
int maxsd;//描述最大值
int isstart,ret_stat,cur_move_sock,target_sock;
int x,y;
int clients[CLIENT_NUM];
int player_on_sock[26];
fd_set liveset, tempset, revset, t_revset;
char table[N][N];
void* start_server(void *arg);
void* listen_net(void *arg);
void init_table();
FILE* logfd;
int main()
{
    // 將SIGPIPE訊號處理方式更改為不處理
    handle_sigpipe();
	
    logfd=fopen("log.txt","w");
	sem_init(&sem,0,1);
	pthread_t thread_listen_net,thread_start_server;
	cur_move_sock=-1,target_sock=0;
	actual_player=0,client_num=0;
	int server_sock,th_ret;
	memset(player_on_sock,0,sizeof(player_on_sock));
	memset(clients,0,sizeof(clients));
	init_table();
	server_sock=create_listen(PORT);
	maxsd=server_sock;//描述最大值
	printf("server start, server_sock=%d\n",server_sock);
	//開始循環監聽
	th_ret=pthread_create(&thread_listen_net, NULL, listen_net, &server_sock); 
	if(th_ret!=0)
	{
		fprintf(logfd,"create thread error!\n");
		fprintf(stderr,"create thread error!\n");
		return -1;
	}
	//創建遊戲
	th_ret=pthread_create(&thread_start_server, NULL, start_server, &server_sock); 
	if(th_ret!=0)
	{
		fprintf(logfd,"create thread error!\n");
		fprintf(stderr,"create thread error!\n");
		return -1;
	}
	pthread_join(thread_listen_net,NULL);
	pthread_join(thread_start_server,NULL);
	fclose(logfd);
	sem_destroy(&sem);
	return 0;
}

void init_table()
{
	for(int i=0;i<N;i++)
	{
		for(int j=0;j<N;j++)
		{
			table[i][j]='_';
		}
	}
}

void* listen_net(void *arg)
{
	int server_sock=*(int*)arg;
	FD_ZERO(&liveset);
	FD_ZERO(&tempset);
	FD_SET(server_sock,&liveset);
	struct sockaddr_in remoteaddr; /* 客戶端地址 */
	socklen_t addrlen;
	while(1)
	{	
		sem_wait(&sem);
		tempset=liveset;
		if((select(maxsd+1,&tempset,NULL,NULL,&timeout))==0)
		{
			sem_post(&sem);
			usleep(50);
			continue;
		}
		for(int i=0;i<=CLIENT_NUM;i++)
		{
			if(clients[i]<=0||i==cur_move_sock)
				continue;
			if(FD_ISSET(clients[i],&tempset))
			{
				char retmsg[256];
				//返回類型：1表示遊戲结束,0表示正常,-1表示錯誤;retmsg表示返回訊息
				ret_stat=deal_player_action(clients[i],&x,&y,retmsg);
				if(ret_stat<=0) //客戶端斷開
				{
					FD_CLR(clients[i], &liveset);
					for(int j=0;j<PLAYER_NUM;j++)//從player_on_sock中刪除sock訊息
					{
						if(player_on_sock[j]==clients[i])
						{
							player_on_sock[j]=-1;
							printf("玩家 %c 退出遊戲\n",(char)j+'A');
							actual_player--;
						}
						
					}
					
					client_num--;
					clients[i]=-1;   //從clients中刪除sock訊息
					continue;
				}
					
			}
			
			
		}
		
		if(FD_ISSET(server_sock,&tempset))//如果是一個新的連接
		{
			int  csd,player_cur; // 已連接的 socket 描述
			csd = accept(server_sock, (struct sockaddr *) &remoteaddr, &addrlen);
			char message[256];
			if(csd==-1)
			{
				fprintf(logfd,"csd=-1,accept:%s\n",strerror(errno));
				perror("csd=-1,accept");
				exit(0);
			}
			
			//發送棋盤訊息
			write(csd,table,sizeof(table));
			//訊息類型type：0表示可以下棋，1表示遊戲狀態更新，2表示提示訊息，-1表示錯誤訊息
			if (client_num>=CLIENT_NUM)
			{
				printf("伺服器人數已滿,已拒絕新的客戶端加入。\n");
				sprintf(message,"伺服器人數已滿！\n");
				send_pak(3,csd,-1,-1,-1,message);
				//發送拒絕訊息
				continue;
				
			}
			else if(actual_player<PLAYER_NUM&&isstart==0)
			{
				for(int j=0;j<=PLAYER_NUM;j++)//從player_on_sock中加入sock訊息
				{
					if(j==PLAYER_NUM)
					{
						fprintf(logfd,"分配玩家位置出錯\n");
						fprintf(stderr,"分配玩家位置出錯\n");
					}
					if(player_on_sock[j]<=0)
					{
						player_cur=j;
						player_on_sock[j]=csd;
						break;
					}
				}
				
				for(int j=0;j<=CLIENT_NUM;j++)//從clients中加入sock訊息
				{
					if(j==CLIENT_NUM)
					{
						fprintf(logfd,"分配玩家位置出錯\n");
						fprintf(stderr,"分配玩家位置出錯b\n");
					}
					if(clients[j]<=0)
					{
						clients[j]=csd;
						break;
					}
				}
				//發送進入成功訊息
				printf("玩家 %c 加入遊戲。\n",(char)player_cur+'A');
				sprintf(message,"加入遊戲成功，你是玩家 %c 。請等待遊戲開始\n",(char)player_cur+'A');
				send_pak(1,csd,-1,-1,(char)player_cur+'A',message);
				//更新伺服器在線玩家和在線伺服器狀態
				actual_player++;
				client_num++;
				
			}
			else 
			{
				for(int j=0;j<=CLIENT_NUM;j++)//從clients中加入sock訊息
				{
					if(j==CLIENT_NUM)
					{
						fprintf(logfd,"分配玩家位置出錯\n");
						fprintf(stderr,"分配玩家位置出錯c\n");
					}
					if(clients[j]<=0)
					{
						clients[j]=csd;
						break;
					}
				}
				
				//發送進入觀戰區訊息
				
				if(isstart==0)
				{
					printf("超過最大玩家數，已把新進入的客戶端拉進觀戰區\n");
					sprintf(message,"超過最大玩家數，已把你拉進觀戰區\n");
				}
				else
				{
					printf("遊戲已開始，已把新進入的客戶端拉進觀戰區！\n");
					sprintf(message,"遊戲已開始，已把你拉進觀戰區！\n");
				}
				
				send_pak(1,csd,-1,-1,(char)0,message);
				
				//更新伺服器在線客戶端狀態
				client_num++;
			}
			
			
			// 將該連接的套接字csd 加入liveset 
			FD_SET(csd, &liveset);
			
			// 保持 maxsd 紀錄的是最大的套接字
			if (csd > maxsd)
				maxsd = csd;
		}
		sem_post(&sem);
		usleep(50);
	}
	
}
void* start_server(void* arg)
{
	while(1)
	{
		int flag=0,server_sock=*(int*)arg;
		char msg[256]="";
		cur_move_sock=-1;
		while(actual_player<=1);
		printf("請按下Enter開始遊戲\n");
		scanf("%*c");
		isstart=1;
		sprintf(msg,"遊戲開始\n");
		for(int i=0;i<CLIENT_NUM;i++)  //逐一發送遊戲開始訊息
		{
			//type(訊息類型)：0表示可以下棋，1表示遊戲狀態更新，2表示提示訊息，-1表示錯誤訊息
			if(player_on_sock[i]<=0)
				continue;
			
			send_pak(1,player_on_sock[i],-1,-1,cur_move_sock+'A',msg);
		}
		sleep(1);
		int t=0;//當前待遍歷的可執行操作的玩家下標
		while(1)
		{
			while(player_on_sock[t]<=0)
			{
				t=(t+1)%PLAYER_NUM;
			}
			cur_move_sock=t;
			
			sem_wait(&sem);//等待客戶端舊的在線情况被刷新
			sem_post(&sem);
			
			if(actual_player<=1)
			{
				sprintf(msg,"玩家數量不足，遊戲结束\n");
				init_table();
				isstart=0;
				for(int i=0;i<CLIENT_NUM;i++)
				{
					if(clients[i]>0)
						send_pak(3,clients[i],x,y,(char)cur_move_sock+'A',msg);
				}
				break;
			}
			char retmsg[256]="";
			//向所有玩家發送訊息
			//type(訊息類型)：0表示可以下棋，1表示遊戲狀態更新，2表示提示訊息，-1表示錯誤訊息
			sprintf(msg,"現在是玩家 %c 的回合\n",cur_move_sock+'A');
			for(int i=0;i<CLIENT_NUM;i++)
			{
				if(clients[i]<=0)
				{
						continue;
				}
				if(player_on_sock[cur_move_sock]==clients[i])
				{
					send_pak(0,clients[i],-1,-1,cur_move_sock+'A',msg);
				}
				else
				{
					send_pak(1,clients[i],-1,-1,cur_move_sock+'A',msg);
				}
			}
			
			ret_stat=deal_player_action(player_on_sock[cur_move_sock],&x,&y,retmsg);
			//ret_stat=0（正常時）的待發送訊息
			if(ret_stat==0)
			{
				table[x][y]=(char)cur_move_sock+'A';
				printf("玩家 %c 把他的棋子落在 ( %d , %d ) 處\n",(char)cur_move_sock+'A',x,y);
				sprintf(msg,"玩家 %c 把他的棋子落在 ( %d , %d ) 處\n",(char)cur_move_sock+'A',x,y);
			}
			
			//若錯誤，直接請求命令的客戶端返回-1
			//錯誤值：-1表示返回錯誤訊息，-2表示非法請求.
			if(ret_stat<0)
			{
				printf("%s",retmsg);
				send_pak(1,player_on_sock[cur_move_sock],-1,-1,(char)cur_move_sock+'A',retmsg);
				if(ret_stat==-1)
					continue;
			}
			else//若無錯誤，則向所有玩家更新訊息
			{
				if(ret_stat==0)
				{
					for(int i=0;i<CLIENT_NUM;i++)
					{
						if(clients[i]>0)
							send_pak(1,clients[i],x,y,(char)cur_move_sock+'A',msg);
					}
					sleep(2);
				}
				if(checkWin(x,y,table,(char)cur_move_sock+'A'))//檢查遊戲是否结束
				{
					for(int i=0;i<CLIENT_NUM;i++)
					{
						if(clients[i]>0)
							send_pak(1,clients[i],x,y,(char)cur_move_sock+'A',msg);
					}
					sprintf(msg,"遊戲结束，玩家 %c 獲勝\n",(char)cur_move_sock+'A');
					for(int i=0;i<CLIENT_NUM;i++)
					{
						if(clients[i]>0)
							send_pak(3,clients[i],x,y,(char)cur_move_sock+'A',msg);
					}
					printf("%s",msg);
					break;
				}
			}
			t=(t+1)%PLAYER_NUM;//當前回合可進行操作的玩家下標更新
		}
	}
}
