#include <sys/types.h>
#include <dirent.h>
#include "../../code/include/csapp.h"

int main(int argc, char **argv)
{
    DIR *streamp;
    struct dirent *dep;

    streamp = opendir("../11network/");

    errno = 0;
    while ((dep = readdir(streamp)) != NULL)
    {
        printf("found file: %s",dep->d_name);
    }
    if (errno != 0)
    {
        fprintf(stderr, "%s: %s\n", "readdir error", strerror(errno));
        exit(0);
    }

    closedir(streamp);
    exit(0);
}