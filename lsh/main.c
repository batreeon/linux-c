#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// read a line of input from stdin
// return the line from stdin
char * lsh_read_line(void)
{
#ifdef LSH_USE_STD_GETLINE
// 使用stdio.h里的getline
    char *line = NULL;
    ssize_t bufsize = 0;
    if (getline(&line, &bufsize, stdin) == -1) {
        if (feof(stdin)) {
            exit(EXIT_SUCCESS);
        } else {
            perror("lsh: getline\n");
            exit(EXIT_FAILURE);
        }
    }
#else
#define LSH_RL_BUFSIZE 1024
    int bufsize = LSH_RL_BUFSIZE;
    // 记录即将往缓冲区里写入的位置
    int position = 0;
    char *buffer = malloc(size(char) * bufsize);
    // 下一个要读入的字符，使用int，因为EOF是int类型
    int c;

    if (!buffer) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    while(1) {
        // 读入字符
        c = getchar();

        if (c == EOF) {
            exit(EXIT_SUCCESS);
        } else if(c == '\n') {
            buffer[position] = '\0';
        } else {
            buffer[position] = c;
        }
        position++;

        if (position >= bufsize) {
            bufsize += LSH_RL_BUFSIZE;
            buffer = realloc(buffer, bufsize);
            if (!buffer) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }
    }
#endif
}

// loop getting input and executing it.
void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    do {
        printf("> ");
        line = lsh_read_line();
        args = lsh_split_line(line);
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);
}

int main(int argc, char **argv)
{
    // load config files, if any

    // run command loop
    
    lsh_loop();

    // perform any shutdown/cleanup

    return EXIT_SUCCESS;
}