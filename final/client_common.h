#ifndef _CLIENT_COMMON_H
#define _CLIENT_COMMON_H

extern int connect_server();
extern void refresh_table();
fd_set revset,t_revset; // 客戶端 sockets

#endif
