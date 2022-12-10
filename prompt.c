#include "prompt.h"
#include "headers.h"

extern char USER[LOGIN_NAME_MAX];
extern char ROOT[HOST_NAME_MAX];
extern char INIT_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;
extern int time_;

void prompt()
{
    printGREEN();
    printf("<%s@%s:", USER, ROOT);

    char cwd[DIR_NAME_MAX] = {'\0'};
    if(getcwd(cwd, DIR_NAME_MAX) == NULL)
    {
        perror("\033[1;31mA-Shell");
        printRESET();
        exit(1);
    }

    int s_cwd = strlen(cwd);

    int printDir; // if(0) print "~", if(-1) it's not a sub-directory of INIT_DIR; if(1) its a sub-directory of INIT_DIR
    if(s_cwd < S_INIT_DIR)
    {
        printDir = -1;
    }

    else if(s_cwd == S_INIT_DIR)
    {
        if(strcmp(cwd, INIT_DIR) == 0)
        {
            printDir = 0;
        }
        else
        {
            printDir = -1;
        }
    }

    else
    {
        if(strncmp(cwd, INIT_DIR, S_INIT_DIR) == 0)
        {
            printDir = 1;
        }
        else
        {
            printDir = -1;
        }
    }
    if(printDir == -1)
    {
        printf("%s", cwd);
    }
    else if(printDir == 0)
    {
        printf("~");
    }
    else
    {
        printf("~%s", cwd + S_INIT_DIR);
    }
    if(time_ != 0)
        printf(" took %lus>", time_);
    else
        printf(">");
    time_ = 0;
    printRESET();
    fflush(stdout);
}