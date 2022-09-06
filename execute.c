#include "execute.h"
#include "headers.h"
#include "cd.h"
#include "ls.h"
#include "history.h"
#include "discover.h"
#include "pinfo.h"

extern char INIT_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;

void execute_cmd(char* command)
{   
    int isbg = 0;
    char* bg_detect = command + strlen(command);
    bg_detect--;
    while(*bg_detect != '&' && *bg_detect != '\0')
    {
        bg_detect--;
    }
    bg_detect++;
    if(*bg_detect == '&')
    {
        int isbg = 1;
    }
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
        temp = strtok(NULL, " \t");
        int num_dirs = 0;
        int num_targets = 0;
        char* __dir = NULL;
        char* target = NULL;
        char cur_dir[DIR_NAME_MAX] = {'\0'};
        int flag = 1; // 1 if no flags, 2 for -d, 3 for -f, 6 for both
        while(temp != NULL)
        {
            if(temp[0] == '-' && strlen(temp) != 1)
            {
                char* fl_string = temp+1;
                int n = strlen(fl_string);
                for(int i = 0; i < n; i++)
                {
                    if(fl_string[i] == 'f')
                    {
                        if(flag%3 != 0)
                        {
                            flag *= 3;
                        }
                    }
                    else if(fl_string[i] == 'd')
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
            else if(temp[0] == '\"' && temp[strlen(temp) - 1] == '\"')
            {
                temp[strlen(temp) - 1] = '\0'; temp++; 
                if(num_targets >= 1)
                {
                    printf(C_ERROR "A-Shell: discover: too many arguments for target\n");
                    printRESET();
                    fflush(stdout);
                    return;
                }
                else
                {
                    num_targets++;
                    target = temp;
                }
            }
            else
            {
                if(num_dirs >= 1)
                {
                    printf(C_ERROR "A-Shell: discover: too many arguments for directory\n");
                    printRESET();
                    fflush(stdout);
                    return;
                }
                else
                {
                    num_dirs++;
                    __dir = temp;
                    if(__dir[0] == '~')
                    {
                        strcpy(cur_dir, INIT_DIR);
                        int n = strlen(__dir);
                        if(n != 1)
                        {
                            strcpy(cur_dir + S_INIT_DIR, __dir + 1);
                        }
                        __dir = &cur_dir[0];
                    }
                }
            }
            temp = strtok(NULL, " \t");
        }
        // if(target == NULL)
        //     printf(C_ERROR"NULL HAI BHAI\n");
        // else
        //     printf(C_ERROR"%s\n", target);
        // printRESET();
        struct stat __dir_stat;
        if(__dir == NULL)
        {
            if((flag != 3) && (target == NULL))
                printf( C_DIR".\n");
            printRESET();
            fflush(stdout);
            A_Shell_discover(__dir, target, flag);
        }
        else if(stat(__dir, &__dir_stat) == -1)
        {
            printf(C_ERROR"A-Shell: discover");
            fflush(stdout);
            perror(C_ERROR);
            return;
        }

        if(__dir != NULL && S_ISDIR(__dir_stat.st_mode))
        {
            if((flag != 3) && (target == NULL))
            {
                printf(C_DIR);
                printf("%s", __dir);
                printRESET();
                fflush(stdout);
            }
            A_Shell_discover(__dir, target, flag);
        }
        else if(__dir != NULL)
        {
            if((__dir_stat.st_mode & S_IXUSR) || (__dir_stat.st_mode & S_IXGRP) || (__dir_stat.st_mode & S_IXOTH))
            {
                printf(C_EXEC);
            }
            else
            {
                printf(C_FILE);
            }
            if((flag != 2) && (target == NULL || strcmp(target, __dir) == 0))    
                printf("%s\n", __dir); 
            printRESET();
            fflush(stdout);     
        } 
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
    else if(strcmp("pinfo", temp) == 0)
    {
        char* pid = NULL;
        temp = strtok(NULL, " \t");
        pid = temp;
        temp = strtok(NULL, " \t");
        if(temp != NULL)
        {
            printf(C_ERROR "A-Shell: history: too many arguments\n");
            printRESET();
            fflush(stdout);
            return;
        }
        if(pid == NULL)
        {
            A_Shell_pinfo(getpid());
            return;
        }
        for(int i = 0; i < strlen(pid); i++)
        {
            if(!isdigit(pid[i]))
            {
                printf(C_ERROR "A-Shell: history: numeric argument required\n");
                printRESET();
                fflush(stdout);
                return;
            }
        }
        A_Shell_pinfo(atoi(pid));
    }
    else
    {
        char* args[MAX_ARGS]; int num_args  = 1;
        args[0] = temp;
        temp = strtok(NULL, " \t");
        while(temp != NULL)
        {
            args[num_args++] = temp;
            temp  = strtok(NULL, " \t");
        }
        
        int pid = fork();
        if(pid == 0)
        {
            int e = execvp(args[0], args);
            if(e == -1)
            {
                printf(C_ERROR "A-Shell: %s: command not found", args[0]);
                printRESET();
                fflush(stdout);    
                perror("");
            }
            exit(2);
        }
        else
        {
            int w;
            waitpid(pid, &w, WUNTRACED);
            return;
            // if(w < )
            // {
                
            // }
        }

        printf(C_ERROR "A-Shell: %s: command not found\n", temp);
        printRESET();
        fflush(stdout);
    }
}