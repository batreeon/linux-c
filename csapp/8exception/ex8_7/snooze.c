#include "../../code/include/csapp.h"

void snooze(int sec) {
    int res = sleep(sec);
    printf("slept %d for %d seconds\n", res, sec);
}

void handler(int sig) {
    // 忽略信号
    printf("\n");
    return;
}

int main(int argc, char **argv) {
    // 当按下ctrl+c时，不会直接退出，而是会打断sleep接着打印换行，打印snooze函数里的输出
    if (signal(SIGINT,&handler) == SIG_ERR) {
        fprintf(stderr, "%s : %s\n", "signal error", strerror(errno));
        exit(0);
    }
    snooze(atoi(argv[1]));
    exit(0);
}