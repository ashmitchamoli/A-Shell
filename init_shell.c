#include "init_shell.h"
#include "headers.h"

extern char USER[LOGIN_NAME_MAX];
extern char ROOT[HOST_NAME_MAX];
extern char INIT_DIR[DIR_NAME_MAX];
extern char PREV_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;

void init_shell()
{
    signal (SIGTTIN, SIG_IGN);
    signal (SIGTTOU, SIG_IGN);
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
    char path[DIR_NAME_MAX] = {'\0'};
    strcpy(path, "/home/");
    strcat(path, USER);
    strcat(path, "/.A_Shell_history.txt\0");
    FILE* f_hist = fopen(path, "a+");
    //printf("%s\n", path);
    if(f_hist == NULL)
    {
        perror(C_ERROR "A-Shell: cannot create history file");
        printRESET();
        fclose(f_hist);
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