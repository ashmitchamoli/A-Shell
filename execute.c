#include "execute.h"
#include "headers.h"
#include "cd.h"
#include "ls.h"
#include "history.h"
#include "discover.h"
#include "pinfo.h"
#include "init_shell.h"

extern time_t time_;
extern char INIT_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;
extern int stdin_cpy;
extern int stdout_cpy;

void execute_cmd(char* command)
{
    //check for redirection here
    char cmd_cpy_1[MAX_CMMD_LEN];
    strcpy(cmd_cpy_1, command);
    int n = strlen(cmd_cpy_1);
    char *infile = NULL, *outfile = NULL;
    int append = 0;
    for(int i = 0; i < n; i++)
    {

        if(cmd_cpy_1[i] == '>')
        {
            command[i] = ' ';
            int j = i + 1;
            if(i < n-1 && cmd_cpy_1[i+1] == '>')
            {
                append = 1;
                command[i + 1] = ' '; 
                j++; i++;
            }
            while(cmd_cpy_1[j] == ' ' || cmd_cpy_1[j] == '\t')
            {
                j++;
            }
            outfile = &(cmd_cpy_1[j]);
            while(j < n && command[j] != ' ' && command[j] != '\t' && command[j] != '>')
            {
                command[j] = ' ';
                j++;
            }
            cmd_cpy_1[j] = '\0';
        }
        else if(cmd_cpy_1[i] == '<')
        {
            command[i] = ' ';
            int j = i+1;
            while(cmd_cpy_1[j] == ' ' || cmd_cpy_1[j] == '\t')
            {
                j++;
            }
            infile = &(cmd_cpy_1[j]);
            while(j < n && command[j]  != '>' && command[j] != ' ' && command[j] != '\t')
            {
                command[j] = ' ';
                j++;
            }
            cmd_cpy_1[j] = '\0';
        }
    }
    if(infile != NULL)
    {
        int in_fd = open(infile, O_RDONLY);
        if(in_fd < 0)
        {
            perror(C_ERROR"A-Shell: redirection");
            return;
        }

        if(dup2(in_fd, STDIN_FILENO) < 0)
        {
            perror(C_ERROR"A-Shell: redirection");
            exit(-1);
        }
        close(in_fd);
    }
    if(outfile != NULL)
    {
        int out_fd;
        if(append)
        {
            out_fd = open(outfile, O_CREAT | O_WRONLY | O_APPEND, 0666);
            if(out_fd < 0)
            {
                perror(C_ERROR"A-Shell: redirection");
                dup2(stdin_cpy, STDIN_FILENO);
                dup2(stdout_cpy, STDOUT_FILENO);
                return;
            }
        }
        else
        {
            out_fd = open(outfile, O_CREAT | O_WRONLY | O_TRUNC, 0666);
            if(out_fd < 0)
            {
                perror(C_ERROR"A-Shell: redirection");
                dup2(stdin_cpy, STDIN_FILENO);
                dup2(stdout_cpy, STDOUT_FILENO);
                return;
            }
        }

        if(dup2(out_fd, STDOUT_FILENO) < 0)
        {
            perror(C_ERROR"A-Shell: redirection");
            exit(-1);
        }
        close(out_fd);
    }

    char cmd_copy[MAX_CMMD_LEN];
    strcpy(cmd_copy, command);
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
            dup2(stdin_cpy, STDIN_FILENO);
            dup2(stdout_cpy, STDOUT_FILENO);
            return;
        }
        char* arg = temp;
        temp = strtok(NULL, " \t");
        if(temp != NULL)
        {
            fprintf(stderr ,C_ERROR "A-Shell: cd: too many arguments\n");
            printRESET();
            fflush(NULL);
            dup2(stdin_cpy, STDIN_FILENO);
            dup2(stdout_cpy, STDOUT_FILENO);
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
            exit(-1);
        }
        printWHITE();
        printf("%s\n", cwd);
        printRESET();
        fflush(NULL);
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
        fflush(NULL);
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
                        fprintf(stderr, C_ERROR "A-Shell: ls: invalid option -- '%c'\n", fl_string[i]);
                        printRESET();
                        fflush(NULL);
                        dup2(stdin_cpy, STDIN_FILENO);
                        dup2(stdout_cpy, STDOUT_FILENO);
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
                        fprintf(stderr ,C_ERROR "A-Shell: ls: invalid option -- '%c'\n", fl_string[i]);
                        printRESET();
                        fflush(NULL);
                        dup2(stdin_cpy, STDIN_FILENO);
                        dup2(stdout_cpy, STDOUT_FILENO);
                        return;
                    }
                }
            }
            else if(temp[0] == '\"' && temp[strlen(temp) - 1] == '\"')
            {
                temp[strlen(temp) - 1] = '\0'; temp++; 
                if(num_targets >= 1)
                {
                    fprintf(stderr ,C_ERROR "A-Shell: discover: too many arguments for target\n");
                    printRESET();
                    fflush(NULL);
                    dup2(stdin_cpy, STDIN_FILENO);
                    dup2(stdout_cpy, STDOUT_FILENO);
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
                    fprintf(stderr ,C_ERROR "A-Shell: discover: too many arguments for directory\n");
                    printRESET();
                    fflush(NULL);
                    dup2(stdin_cpy, STDIN_FILENO);
                    dup2(stdout_cpy, STDOUT_FILENO);
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
            fflush(NULL);
            A_Shell_discover(__dir, target, flag);
        }
        else if(stat(__dir, &__dir_stat) == -1)
        {
            fprintf(stderr ,C_ERROR"A-Shell: discover");
            fflush(NULL);
            perror(C_ERROR);
            printRESET();
            dup2(stdin_cpy, STDIN_FILENO);
            dup2(stdout_cpy, STDOUT_FILENO);
            return;
        }

        if(__dir != NULL && S_ISDIR(__dir_stat.st_mode))
        {
            if((flag != 3) && (target == NULL))
            {
                printf(C_DIR);
                printf("%s", __dir);
                printRESET();
                fflush(NULL);
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
            fflush(NULL);     
        } 
    }
    else if(strcmp("history", temp) == 0)
    {
        temp = strtok(NULL, " \t");
        if(temp != NULL)
        {
            fprintf(stderr ,C_ERROR "A-Shell: history: too many arguments\n");
            printRESET();
            fflush(NULL);
            dup2(stdin_cpy, STDIN_FILENO);
            dup2(stdout_cpy, STDOUT_FILENO);
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
            fprintf(stderr ,C_ERROR "A-Shell: pinfo: too many arguments\n");
            printRESET();
            fflush(NULL);
            dup2(stdin_cpy, STDIN_FILENO);
            dup2(stdout_cpy, STDOUT_FILENO);
            return;
        }
        if(pid == NULL)
        {
            A_Shell_pinfo(getpid());
            dup2(stdin_cpy, STDIN_FILENO);
            dup2(stdout_cpy, STDOUT_FILENO);
            return;
        }
        for(int i = 0; i < strlen(pid); i++)
        {
            if(!isdigit(pid[i]))
            {
                fprintf(stderr ,C_ERROR "A-Shell: pinfo: numeric argument required\n");
                printRESET();
                fflush(NULL);
                dup2(stdin_cpy, STDIN_FILENO);
                dup2(stdout_cpy, STDOUT_FILENO);
                return;
            }
        }
        A_Shell_pinfo(atoi(pid));
    }
    else if(strcmp("jobs", temp) == 0)
    {
        
    }
    else
    {
        char* args[MAX_ARGS]; int num_args  = 1;
        temp = strtok(cmd_copy, " \t");
        args[0] = temp;
        temp = strtok(NULL, " \t");
        while(temp != NULL)
        {
            args[num_args++] = temp;
            temp  = strtok(NULL, " \t");
        }
        int fork_id = fork();
 
        if(fork_id == 0)
        {
            int pid = getpid();
            setpgid(pid, pid);
            tcsetpgrp(STDIN_FILENO, pid);
            signal (SIGINT, SIG_DFL);
            signal (SIGTTIN, SIG_DFL);
            signal (SIGTTOU, SIG_DFL);
            args[num_args] = NULL;
            execvp(args[0], args);
            fprintf(stderr, C_ERROR "A-Shell: %s: command not found", args[0]);
            printRESET();
            fflush(NULL);
            perror(C_ERROR"");
            printRESET();
            exit(2);
        }
        else
        {
            int pid = getpid();
            setpgid(pid, pid);
            int w;
            time_t t1 = time(NULL);
            waitpid(fork_id, &w, WUNTRACED);
            time_t t2 = time(NULL);
            time_ = t2 - t1;
            tcsetpgrp(STDIN_FILENO, pid);
            signal (SIGINT, SIG_IGN);
            signal (SIGTTIN, SIG_IGN);
            signal (SIGTTOU, SIG_IGN);
        }
    }
    dup2(stdin_cpy, STDIN_FILENO);
    dup2(stdout_cpy, STDOUT_FILENO);
}