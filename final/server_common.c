#include<stdio.h>
#include"common.h"
#include"server_common.h"
//extern int maxsd;
extern FILE* logfd;

int create_listen(int port)  //創建監聽
{
	if(CLIENT_NUM < MIN_CLIENT_NUM || CLIENT_NUM > MAX_CLIENT_NUM)
	{
		fprintf(logfd, "CLIENT_NUM is more than %d or less than %d,please check.",
                MAX_CLIENT_NUM, MIN_CLIENT_NUM);
		fprintf(stderr, "CLIENT_NUM is more than %d or less than %d,please check.",
                MAX_CLIENT_NUM, MIN_CLIENT_NUM);
		exit(1);
	}

	if(PLAYER_NUM < MIN_PLAYER_NUM || PLAYER_NUM > MAX_PLAYER_NUM)
	{
		fprintf(logfd, "PLAYER_NUM is more than %d or less than %d,please check.",
                MAX_PLAYER_NUM, MIN_PLAYER_NUM);
		fprintf(stderr, "PLAYER_NUM is more than %d or less than %d,please check.",
                MAX_PLAYER_NUM, MIN_PLAYER_NUM);
		exit(1);
	}
	int sock;
	SA_in sa;
	/*設置通訊類型*/
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		fprintf(logfd,"socket create error!");
		fprintf(stderr,"socket create error!");
		exit(1);
	}
	
	/*設置通訊參數*/
	sa.sin_family=AF_INET; //協議，IPV4
	sa.sin_addr.s_addr=htonl(INADDR_ANY); //IP地址，INADDR_ANY表示接受任意IP連接
	sa.sin_port=htons(PORT);    //端口，在common.h裡設置
	
	if((bind(sock,(pSA)&sa,sizeof(sa)))==-1) //將sock和通訊參數绑定
	{
		fprintf(logfd,"socket create error!:%s",strerror(errno));
		fprintf(stderr,"socket bind error!");
		exit(1);
	}
	
	if((listen(sock,CLIENT_NUM+1))==-1)  //player_num：玩家數，在common.h裡設置
	{
		fprintf(logfd,"socket listen error!:%s",strerror(errno));
		fprintf(stderr,"socket listen error!");
		exit(1);
	}
	return sock;
}

int deal_player_action(int client_sock,int* retx,int* rety,char retmsg[])
//返回類型：0表示正常,-1表示返回錯誤訊息，-2表示非法請求;retmsg表示返回訊息
{
	FD_ZERO(&revset);
	FD_ZERO(&t_revset);
	FD_SET(client_sock,&revset);
	//MSGPAK 數據，在 common.h 中定義
	MSGPAK* msg_recv=(MSGPAK*)malloc(sizeof(MSGPAK));
	while(1)
	{
		t_revset=revset;
		select(client_sock+1,&t_revset,NULL,NULL,NULL);
		if(FD_ISSET(client_sock,&t_revset))
			break;
	}
	if((read(client_sock,msg_recv,sizeof(MSGPAK)))<=0)
	{
		sprintf(retmsg,"請求錯誤，請重試\n");
		return -2;
	}
	*retx=msg_recv->x,*rety=msg_recv->y;//更新返回座標
	if(msg_recv->x<0||msg_recv->x>=N||msg_recv->y<0||msg_recv->y>=N)//檢查座標合法性
	{
		sprintf(retmsg,"座標錯誤，請重試\n");
		
		return -1;
	}
	else if(table[msg_recv->x][msg_recv->y]!='_')//檢查該座標上有無棋子
	{
		sprintf(retmsg,"該座標上有棋子了，請重試\n");
		return -1;
	}
	return 0;
}

