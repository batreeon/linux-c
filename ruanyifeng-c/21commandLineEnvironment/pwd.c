#include <unistd.h>

int main() {
    char *pwd = getcwd(NULL, 0);
    printf("%s>", pwd);
}