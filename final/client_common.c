#include<stdio.h>
#include"common.h"

//客戶端連接到伺服器
 
int connect_server(char * addr,int port)
{
	int sock;
	SA_in sa; // struct sockaddr_in, 在common.h中定義
	
	/*設置通訊類型*/
	if((sock=socket(AF_INET,SOCK_STREAM,0))==-1)
	{
		fprintf(stderr,"socket create error!");
		exit(1);
	}
	
	/*設置通訊參數*/
	sa.sin_family=AF_INET; //協議，IPV4
	sa.sin_addr.s_addr=inet_addr(IP); //IP地址
	sa.sin_port=htons(PORT);    //端口
	if((connect(sock,(pSA)&sa,sizeof(sa)))==-1)
	{
		fprintf(stderr,"fail to connect server!");
		exit(1);
	}
	
	return sock;
}

//更新棋盤
void refresh_table()
{
	system("clear");
	printf("  ");
	for(int i=0;i<N;i++)
	{
		printf("%d ",i);
	}
	printf("\n");
	for(int i=0;i<N;i++)
	{
		printf("%d ",i);
		for(int j=0;j<N;j++)
		{
			printf("%c ",table[i][j]);
		}
		printf("\n");
	}
}

