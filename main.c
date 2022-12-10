#include "headers.h"
#include "prompt.h"
#include "init_shell.h"
#include "input.h"
#include "execute.h"

extern int num_cmmds;
extern int stdin_cpy;
extern int stdout_cpy;

int main()
{
    init_shell();
    prompt();

    while(1)
    {
        char* inp = get_input();
        if(inp == NULL)
        {
            prompt();
            continue;
        }
        char** list = get_commands(inp);
        if(list == NULL)
        {
            prompt();
            continue;
        }
        for(int i = 0; i < num_cmmds; i++)
        {
            int num_pipes = 0;

            // count pipes
            int n_listi = strlen(list[i]);
            for(int j = 0; j < n_listi; j++)
            {
                if(list[i][j] == '|')
                {
                    num_pipes++;
                }
            }

            if(num_pipes == 0)
            {
                execute_cmd(list[i]);
                continue;
            }

            char* cmmds[MAX_CMMDS];
            // tokenise wrt '|'
            char* temp = strtok(list[i], "|");
            int n_cmmds = 0;
            while(temp != NULL)
            {
                cmmds[n_cmmds++] = temp;
                temp = strtok(NULL, "|");
            }

            int stdout_master;
            if((stdout_master = dup(STDOUT_FILENO)) < 0)
            {
                perror(C_ERROR"5A-Shell");
                exit(-1);
            }
            int stdin_master;
            if((stdin_master = dup(STDIN_FILENO)) < 0)
            {
                perror(C_ERROR"5A-Shell");
                exit(-1);
            }
            
            int pipe_fd[2];
            for(int j = 0; j < n_cmmds-1; j++)
            {
                pipe(pipe_fd);
                int fork_id = fork();
                if(fork_id == 0)
                {
                    setpgrp();
                    tcsetpgrp(STDIN_FILENO, getpid());
                    close(pipe_fd[0]);
                    dup2(pipe_fd[1], STDOUT_FILENO);
                    close(pipe_fd[1]);
                    execute_cmd(cmmds[j]);
                    exit(0);
                }
                else
                {
                    setpgid(fork_id, fork_id);
                    tcsetpgrp(STDIN_FILENO, fork_id);
                    int w;
                    waitpid(fork_id, &w, WUNTRACED);
                    close(pipe_fd[1]);
                    tcsetpgrp(STDIN_FILENO, getpid());
                    dup2(pipe_fd[0], STDIN_FILENO);
                    close(pipe_fd[0]);
                }
            }
            int fork_id = fork();
            if(fork_id == 0)
            {
                setpgrp();
                tcsetpgrp(STDIN_FILENO, getpid());
                close(stdin_master);
                dup2(stdout_master, STDOUT_FILENO);
                close(stdout_master);
                execute_cmd(cmmds[n_cmmds-1]);
                exit(0);
            }
            else
            {
                setpgid(fork_id, fork_id);
                tcsetpgrp(STDIN_FILENO, fork_id);
                int w; 
                waitpid(fork_id, &w, WUNTRACED);
                close(stdout_master);
                tcsetpgrp(STDIN_FILENO, getpid());
                dup2(stdin_master, STDIN_FILENO);
                close(stdin_master);
            }
        }
        free(inp);
        free(list);
        prompt();
    }
}