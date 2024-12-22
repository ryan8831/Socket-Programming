#include <signal.h>
#include <errno.h>
#include <stdio.h>

void sigpipe_handler();

void handle_sigpipe()
{
    struct sigaction action;
    action.sa_handler = sigpipe_handler;
    sigemptyset(&action.sa_mask);
    action.sa_flags = 0;
    sigaction(SIGPIPE, &action, NULL);
}

void sigpipe_handler()
{
    perror("發生了一個sigpipe, 錯誤訊息：");
}

