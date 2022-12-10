#include "cd.h"
#include "headers.h"

extern char PREV_DIR[DIR_NAME_MAX];
extern char INIT_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;
void A_Shell_cd(char* arg)
{
    char cwd[DIR_NAME_MAX] = {'\0'};
    if(getcwd(cwd, DIR_NAME_MAX) == NULL)
    {
        perror(C_ERROR "A-Shell");
        printRESET();
        return;
    }

    char final_dir[DIR_NAME_MAX];

    if(strcmp("-", arg) == 0)
    {
        if(strcmp(PREV_DIR, "") == 0)
        {
            fprintf(stderr ,C_ERROR "A-Shell: cd: PREV_DIR not set\n");
            printRESET();

            fflush(stdout);
            return;
        }
        printWHITE();
        printf("%s\n", PREV_DIR);
        printRESET();
        
        fflush(stdout);
        strcpy(final_dir, PREV_DIR);
    }
    else if(arg[0] == '~')
    {
        strcpy(final_dir, INIT_DIR);
        int n = strlen(arg);
        if(n != 1)
        {
            strcpy(final_dir + S_INIT_DIR, arg+1);
        }
    }
    else
    {
        strcpy(final_dir, arg);
    }

    if(chdir(final_dir) == -1)
    {
        perror(C_ERROR "A-Shell: cd");
        printRESET();
        fflush(stdout);
        return;
    }
    strcpy(PREV_DIR, cwd);
}