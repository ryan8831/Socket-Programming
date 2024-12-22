#include"common.h"
//extern FILE* logfd;

void send_pak(int type, int sock, int x, int y, char player_tpye, char* message)
//發送數據
{
	MSGPAK* msg_send =(MSGPAK*)malloc(sizeof(MSGPAK));
	msg_send->type=type;
	msg_send->isstart=isstart;
	msg_send->sock=sock;
	msg_send->x=x;
	msg_send->y=y;
	msg_send->player_tpye=player_tpye;
	strcpy(msg_send->message,message);
	write(sock,msg_send,sizeof(MSGPAK));
	free(msg_send);
}

