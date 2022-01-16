#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// 内建命令声明
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);

// 内建命令列表
char *builtin_str[] = {
    "cd",
    "help",
    "exit"
};

// 内建命令对应的函数列表
int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_help,
    &lsh_exit,
};

// 返回内建命令数量
int lsh_num_builtins() 
{
    return sizeof(builtin_str) / sizeof(char *);
}

// cd 命令
// 返回1,继续executing
int lsh_cd(char **args)
{
    if (args[1] == NULL) {
        fprintf(stderr, "lsh: expected argument to \"cd\"\n");
    } else {
        if (chdir(args[1]) != 0) {
            perror("lsh");
        }
    }
    return 1;
}

int lsh_help(char **args)
{
    printf("hdyxr's LSH\n");
    printf("Type program names and arguments, and hit enter.\n");
    printf("The following are built in:\n");

    for(int i = 0; i < lsh_num_builtins(); i++) {
        printf(" %s\n", builtin_str[i]);
    }

    printf("Use the man command for information on other program.\n");
    return 1;
}

int lsh_exit(char **args)
{
    return 0;
}

// 启动一个程序并等待其结束
int lsh_launch(char **args)
{
    pid_t pid;
    int status;

    pid = fork();
    if(pid == 0) {
        // child process
        if(execvp(args[0], args) == -1) {
            perror("lsh");
        }
        exit(EXIT_FAILURE);
    } else if (pid < 0) {
        // error
        perror("lsh");
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}


// 执行shell内建命令或者启动程序
int lsh_execute(char **args)
{
    if (args[0] == NULL) {
        // 空命令
        return 1;
    }

    for (int i = 0; i < lsh_num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    return lsh_launch(args);
}

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

    return line;
#else
#define LSH_RL_BUFSIZE 1024
    int bufsize = LSH_RL_BUFSIZE;
    // 记录即将往缓冲区里写入的位置
    int position = 0;
    char *buffer = malloc(sizeof(char) * bufsize);
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
            return buffer;
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

// 解析读取行
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM "\t\r\n\a"
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    char **tokens = malloc(bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    token = strtok(line, LSH_TOK_DELIM);
    while (!token) {
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        token = strtok(NULL, LSH_TOK_DELIM);
    }

    // 作为结束的标记
    tokens[position] = NULL;
    return tokens;
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