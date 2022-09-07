#include "input.h"
#include "headers.h"
#include "process_list.h"
#include "bg_handler.h"

extern int num_cmmds;
extern char USER[LOGIN_NAME_MAX];
extern p_list* head;
extern int num_bg;

int bg_create(char* cmd)
{
    char* temp = strtok(cmd, " \t");
    if(temp == NULL)
    {
        return -1;
    }
    char* args[MAX_ARGS];
    int _num_args = 1;
    args[0] = temp;
    temp = strtok(NULL, " \t");
    while(temp != NULL)
    {
        args[_num_args++] = temp;
        temp = strtok(NULL, " \t");
    }

    int fork_id = fork();

    if(fork_id == 0)
    {
        int pid = getpid();
        setpgid(pid, pid);
        signal (SIGINT, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        args[_num_args] = NULL;
        execvp(args[0], args);
        fprintf(stderr, C_ERROR "A-Shell: %s: command not found", args[0]);
        printRESET();
        fflush(stdout);
        perror(C_ERROR"");
        printRESET();
        exit(2);
    }
    else
    {
        signal (SIGINT, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN); 
        add_list(cmd, fork_id);
        num_bg++;
        printCYAN();
        printf("[%d] %d\n", num_bg, fork_id);
        printRESET();
        fflush(stdout);
        int pid = getpid();
        setpgid(pid, pid);
        tcsetpgrp (STDIN_FILENO, pid);
        signal(SIGCHLD, bg_handler);
        return 0;
    }
}

void handle_history(char* inp)
{
    int isEmpty = 1;
    for(int i = 0; i < strlen(inp); i++)
    {
        if(inp[i] != ' ' && inp[i] != '\t')
        {
            isEmpty = 0;
            break;
        }
    }
    int __n = strlen(inp);
    inp[__n] = '\n';
    char hist[MAX_HIST][MAX_CMMD_LEN];
    int num_ent = 0;
    char path[DIR_NAME_MAX] = {'\0'};
    strcpy(path, "/home/");
    strcat(path, USER);
    strcat(path, "/.A_Shell_history.txt\0");
    FILE* f_hist = fopen(path, "r");
    if(f_hist == NULL)
    {
        perror(C_ERROR "A-Shell: cannot read history file");
        exit(1);
    }
    while(fgets(hist[num_ent], MAX_CMMD_LEN, f_hist) != NULL)
    {
        num_ent++;
    }
    fclose(f_hist);
    inp[__n + 1] == '\0';
    if(isEmpty || strcmp(hist[num_ent-1], inp) == 0)
    {
        inp[__n] = '\0';
        return;
    }
    f_hist = fopen(path, "w");
    if(num_ent > MAX_HIST)
    {
        fprintf(stderr ,C_ERROR "A-Shell: files tampered");
        fclose(f_hist);
        exit(1);
    }
    else if(num_ent == MAX_HIST)
    {
        int w;
        for(int i = 1; i < MAX_HIST; i++)
        {
            w = fputs(hist[i], f_hist);
            if(w == EOF)
            {
                perror(C_ERROR "A-Shell: error writing history file");
                fclose(f_hist);
                exit(1);
            }
        }
        inp[__n + 1] = '\0';
        w = fputs(inp, f_hist);
        if(w == EOF)
        {
            perror(C_ERROR "A-Shell: error writing history file");
            fclose(f_hist);
            exit(1);
        }
    }
    else
    {
        inp[__n + 1] = '\0';
        int w;
        for(int i = 0; i < num_ent; i++)
        {
            w = fputs(hist[i], f_hist);
            if(w == EOF)
            {
                perror(C_ERROR "A-Shell: error writing history file");
                fclose(f_hist);
                exit(1);
            }
        }
        inp[__n + 1] = '\0';
        w = fputs(inp, f_hist);
        if(w == EOF)
        {
            perror(C_ERROR "A-Shell: error writing history file");
            fclose(f_hist);
            exit(1);
        }
    }
    inp[__n] = '\0';
    fclose(f_hist);
}

char* get_input()
{
    char* inp = NULL;
    size_t alloc_inp; //size allocated to inp
    ssize_t s_inp = getline(&inp, &alloc_inp, stdin);
    if(s_inp == -1)
    {
        perror("\033[1;31mA-Shell");
        printRESET();
        exit(1);
    }
    if(s_inp > MAX_CMMD_LEN - 1)
    {
        fprintf(stderr ,C_ERROR "A-Shell: input length too long");
        return NULL;
    }
    if(inp[s_inp - 1] == '\n') inp[s_inp - 1] = '\0';
    return inp;
}

char** get_commands(char* inp)
{
    // history handling
    handle_history(inp);

    num_cmmds = 0; //to count the number of commands
    char* temp = strtok(inp, ";");
    if(temp == NULL)
    {
        return NULL;
    }
    char** commands = (char**) malloc(sizeof(char*)*MAX_CMMDS);
    while(temp != NULL)
    {
        commands[num_cmmds++] = temp;
        temp = strtok(NULL, ";");
    }
    char** shell_commands = (char**) malloc(sizeof(char*)*MAX_CMMDS);
    int num_shell_cmmds = 0;
    for(int i = 0; i < num_cmmds; i++)
    {
        char* prev = commands[i]; int n = strlen(commands[i]);
        for(int j = 0; j < n; j++)
        {
            if(commands[i][j] == '&')
            {
                commands[i][j] = '\0';
                bg_create(prev);
                prev = &(commands[i][j+1]);
            }
        }
        shell_commands[num_shell_cmmds++] = prev;
    }
    free(commands);
    num_cmmds = num_shell_cmmds;
    return shell_commands;
}