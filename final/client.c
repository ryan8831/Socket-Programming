#include "common.h"
#include "client_common.h"
int isstart=0;
char table[N][N];
FILE* logfd;
int main()
{
	logfd=fopen("clientlog.txt","w");
	char msg[256];
	MSGPAK* pak=calloc(1,sizeof(MSGPAK));
	int sock=0,ret,x=-1,y=-1;
	sock=connect_server(IP,PORT);//IP和port:在common.h中設置
	if(sock==-1)
	{
		fprintf(stderr,"連接伺服器失敗\n");
		perror("error");
		return -1;
	}
	printf("連接伺服器成功\n");
	//獲取棋盤訊息
	if((read(sock,table,sizeof(table)))<=0)
	{
		fprintf(stderr,"初始化棋盤失敗");
		return -1;
	}
	
	while(1)
	{
		FD_ZERO(&revset);
		FD_ZERO(&t_revset);
		FD_SET(sock,&revset);
		
		while(1)
		{
			t_revset=revset;
			select(sock+1,&t_revset,NULL,NULL,NULL);
			if(FD_ISSET(sock,&t_revset))
				break;
		}
		if((read(sock,pak,sizeof(MSGPAK)))<=0)
		{
            fprintf(stderr, "讀取伺服器訊息失敗"); 
            if(errno == EPIPE)
            {
               fprintf(stderr, "伺服器連接意外關閉\n"); 
            }
            else
            {
			    perror("錯誤訊息：");
            }
			exit(-1);
		}
		//讀取數據類型
		//類型(type)：0表示可以下棋，1表示遊戲狀態更新，2表示提示訊息,3表示遊戲结束，-1表示錯誤訊息
		if(!pak)
		{
			fprintf(stderr,"讀取伺服器訊息失敗\n");
		}
		if(pak->type==-1)
		{
			refresh_table();
			fprintf(stderr,"%s",pak->message);
			exit(-1);
		}
		else if(pak->type==1)
		{
			if(pak->x>=0&&pak->x<N&&pak->y>=0&&pak->y<N)
			{
				table[pak->x][pak->y]=pak->player_tpye;
				usleep(500);
			}
			if(pak->isstart==1)
				refresh_table();
			printf("%s",pak->message);
		}
		else if(pak->type==2)
		{
			printf("%s",pak->message);
			
		}
		else if(pak->type==0)
		{
			//printf("type=%d\n",pak->type);
			printf("%s",pak->message);
			printf("現在是你的回合，開始下棋了(格式：x y （注意x和y之間有空格）)：\n");
			scanf("%d %d",&x,&y);
            // 清空緩衝
            char ch;
            while((ch = getchar()) != '\n' && ch != EOF);
            printf("座標：%d %d\n", x, y);
			send_pak(1,sock,x,y,'A',msg);
		}
		else if(pak->type==3)
		{
			table[pak->x][pak->y]=pak->player_tpye;
			printf("%s",pak->message);
			break;
		}
        else
        {
            usleep(500);
        }
	}
	fclose(logfd);
	return 0;
}

