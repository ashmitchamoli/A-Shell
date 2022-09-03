#include "init_shell.h"
#include "headers.h"

extern char USER[LOGIN_NAME_MAX];
extern char ROOT[HOST_NAME_MAX];
extern char INIT_DIR[DIR_NAME_MAX];
extern char PREV_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;

void init_shell()
{
    FILE* f_hist = fopen("/tmp/A-Shell_history.txt", "a+");
    if(f_hist == NULL)
    {
        perror(C_ERROR "A-Shell: cannot create '/tmp/A-Shell_history.txt'");
        printRESET();
    }
    if(gethostname(ROOT, HOST_NAME_MAX) == -1)
    {
        perror("\033[1;31mA-Shell");
        printRESET();
        exit(1);
    }
    if(getlogin_r(USER, LOGIN_NAME_MAX) == -1)
    {
        perror("\033[1;31mA-Shell");
        printRESET();
        exit(1);
    }
    memset(INIT_DIR, '\0', DIR_NAME_MAX);
    if(getcwd(INIT_DIR, DIR_NAME_MAX) == NULL)
    {
        perror("\033[1;31mA-Shell");
        printRESET();
        exit(1);
    }
    S_INIT_DIR = strlen(INIT_DIR);
    memset(PREV_DIR, '\0', DIR_NAME_MAX);
}