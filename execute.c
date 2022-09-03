#include "execute.h"
#include "headers.h"
#include "cd.h"
#include "ls.h"
#include "history.h"

void execute_cmd(char* command)
{
    char* temp = strtok(command, " \t");
    if(temp == NULL)
    {
        return;
    }
    if(strcmp(temp, "cd") == 0)
    {
        temp = strtok(NULL, " \t");
        if(temp == NULL)
        {
            A_Shell_cd("~");
            return;
        }
        char* arg = temp;
        temp = strtok(NULL, " \t");
        if(temp != NULL)
        {
            printf(C_ERROR "A-Shell: cd: too many arguments\n");
            printRESET();
            fflush(stdout);
            return;
        }
        A_Shell_cd(arg);
    }
    else if(strcmp(temp, "pwd") == 0)
    {
        char cwd[DIR_NAME_MAX] = {'\0'};
        if(getcwd(cwd, DIR_NAME_MAX) == NULL)
        {
            perror(C_ERROR "A-Shell");
            printRESET();
            exit(1);
        }
        printWHITE();
        printf("%s\n", cwd);
        printRESET();
        fflush(stdout);
    }
    else if(strcmp(temp, "echo") == 0)
    {
        temp = strtok(NULL, " \t");
        printWHITE();
        while(temp != NULL)
        {
            printf("%s", temp);
            temp = strtok(NULL, " \t");
            if(temp != NULL)
            {
                printf(" ");
            }
        }
        printf("\n");
        printRESET();
        fflush(stdout);
    }
    else if(strcmp(temp, "ls") == 0)
    {
        temp = strtok(NULL, " \t");
        int num_dirs = 0;
        char* dirs[MAX_DIR_LS];
        int flag = 1; // 1 if no flags, 2 for -l, 3 for -a, 6 for both
        while(temp != NULL)
        {
            if(temp[0] == '-' && strlen(temp) != 1)
            {
                char* fl_string = temp+1;
                int n = strlen(fl_string);
                for(int i = 0; i < n; i++)
                {
                    if(fl_string[i] == 'a')
                    {
                        if(flag%3 != 0)
                        {
                            flag *= 3;
                        }
                    }
                    else if(fl_string[i] == 'l')
                    {
                        if(flag%2 != 0)
                        {
                            flag *= 2;
                        }
                    }
                    else
                    {
                        printf(C_ERROR "A-Shell: ls: invalid option -- '%c'\n", fl_string[i]);
                        printRESET();
                        fflush(stdout);
                        return;
                    }
                }
            }
            else
            {
                dirs[num_dirs++] = temp;
            }
            temp = strtok(NULL, " \t");
        }
        A_Shell_ls(dirs, num_dirs, flag);
    }
    else if(strcmp("discover", temp) == 0)
    {

    }
    else if(strcmp("history", temp) == 0)
    {
        temp = strtok(NULL, " \t");
        if(temp != NULL)
        {
            printf(C_ERROR "A-Shell: history: too many arguments\n");
            printRESET();
            fflush(stdout);
            return;
        }
        A_Shell_history();
    }
    else
    {
        printf(C_ERROR "A-Shell: %s: command not found\n", temp);
        printRESET();
        fflush(stdout);
    }
}