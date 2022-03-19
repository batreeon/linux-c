#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <pwd.h>

// 内建命令声明
int lsh_cd(char **args);
int lsh_help(char **args);
int lsh_exit(char **args);
int lsh_pwd(char **args);

// 内建命令列表
char *builtin_str[] = {
    "cd",
    "pwd",
    "help",
    "exit"
};

// 内建命令对应的函数列表
// builtin_func是一个数组指针，数组元素是指针，数组元素指向函数，函数参数是字符串，函数返回int
int (*builtin_func[]) (char **) = {
    &lsh_cd,
    &lsh_pwd,
    &lsh_help,
    &lsh_exit,
};

// 返回内建命令数量
int lsh_num_builtins() 
{
    return sizeof(builtin_str) / sizeof(char *);
}

// cd 命令
// cd ~
// cd a b
// cd ~ a
// 返回1,继续executing
int lsh_cd(char **args)
{
    if (!args[1]) {
        fprintf(stderr, "lsh : expected argument to \"cd\"\n");
    } else if (args[3]) {
        fprintf(stderr, "cd: too many arguments\n");
    }else if (args[2]) {
        char *target = getcwd(NULL, 0);
        if (target == NULL) {
            perror("lsh");
        }

        if (strcmp(args[1], "~") == 0) {
            char *home = getenv("HOME");
            args[1] = home;
        }

        char *pos = strstr(target, args[1]);
        if (pos) {
            size_t lenArgs1 = strlen(args[1]);
            size_t lenArgs2 = strlen(args[2]);
            strcpy(pos+lenArgs2, pos+lenArgs1);
            // args[2]末尾有 '\n'
            strncpy(pos, args[2], lenArgs2);
            // printf("%s\n", target);
            if (chdir(target) != 0) {
                fprintf(stderr, "cd: no such file or directory: %s\n", target);
            }
        }else{
            fprintf(stderr, "cd: string not in pwd: %s\n", args[1]);
        }
    }else{
        if (strcmp(args[1], "~") == 0) {
            args[1] = getenv("HOME");
        }
        if (chdir(args[1]) != 0) {
            fprintf(stderr, "cd: no such file or directory: %s\n", args[1]);
        }
    }
    return 1;
}

int lsh_pwd(char **args) {
    if (args[1] != NULL) {
        fprintf(stderr, "pwd: too many arguments\n");
    }else{
        char *pwd = getcwd(NULL, 0);
        if (pwd == NULL) {
            perror("lsh");
        }else{
            puts(pwd);
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
// 分隔符
#define LSH_TOK_DELIM " \t\r\n\a"
char **lsh_split_line(char *line)
{
    int bufsize = LSH_TOK_BUFSIZE;
    int position = 0;
    // 字符串数组
    char **tokens = malloc(bufsize * sizeof(char*));
    // malloc不会对分配的空间进行初始化，我们后面会通过第四个参数是否为空，
    // 来判断cd 命令参数是否过多，因此memset将所有元素初始化为0
    memset(tokens, 0, bufsize * sizeof(char*));
    char *token;

    if (!tokens) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    // 分割字符串
    token = strtok(line, LSH_TOK_DELIM);
    while (token != NULL) {
        // printf("token: %s ", token);
        tokens[position] = token;
        position++;

        if (position >= bufsize) {
            bufsize += LSH_TOK_BUFSIZE;
            // 为什么要这样？？因为元素是字符串（字符数组）？？
            char **tokens_backup = tokens;
            tokens = realloc(tokens, bufsize * sizeof(char*));

            if (!tokens) {
                free(tokens_backup);
                fprintf(stderr, "lsh: allocation error\n");
                exit(EXIT_FAILURE);
            }
        }

        // 下一个字符串
        token = strtok(NULL, LSH_TOK_DELIM);
    }
    // printf("\n");

    // 作为结束的标记
    tokens[position] = NULL;
    return tokens;
}

// 执行shell内建命令或者启动程序
int lsh_execute(char **args)
{
    if (args[0] == NULL) {
        // 空命令
        return 1;
    }

    // 内建命令
    for (int i = 0; i < lsh_num_builtins(); i++) {
        if(strcmp(args[0], builtin_str[i]) == 0) {
            return (*builtin_func[i])(args);
        }
    }

    // return lsh_launch(args);
    printf("lsh: %s %s\n", "command not found:", args[0]);
    return 1;
}

// 命令行提示符
void cliPrompt(char *hostname) {
    // 获取用户名
    struct passwd *password = getpwuid(getuid());
    char *username = password->pw_name;

    // 获取主机名
    if (gethostname(hostname, 512) == -1) {
        perror("gethostname");
    }

    // 获取用户主目录
    char *home = getenv("HOME");
    size_t homePathLen;
    if (!home) {
        // 
        printf("connot find the HOME environment variable\n");
    }else{
        homePathLen = strlen(home);
    }

    // 获得当前路径
    char *pwd = getcwd(NULL, 0);
    // 查看是否在主目录下,若是，将home替换为~
    if (strncmp(pwd, home, homePathLen) == 0) {
        strcpy(pwd+1, pwd+homePathLen);
        pwd[0] = '~';
    }

    // 打印命令行提示符
    printf("[%s@%s] %s> ", username, hostname, pwd);
}

// loop getting input and executing it.
void lsh_loop(void)
{
    char *line;
    char **args;
    int status;

    char *hostname = malloc(512 * sizeof(char));
    if (!hostname) {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
    }

    do {
        cliPrompt(hostname);

        // 读取一行
        line = lsh_read_line();
        // printf("input line: %s\n", line);
        // 解析行输入，获得分割后的字符串数组
        args = lsh_split_line(line);
        // int pos = 0;
        // for(; args[pos] != NULL; pos++) {
        //     printf("args[%d]: %s ", pos, args[pos]);
        // }
        // printf("\n");
        status = lsh_execute(args);

        free(line);
        free(args);
    } while (status);

    free(hostname);
}

int main(int argc, char **argv)
{
    // load config files, if any

    // run command loop
    
    lsh_loop();

    // perform any shutdown/cleanup

    return EXIT_SUCCESS;
}