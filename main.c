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
                if((stdin_cpy = dup(STDIN_FILENO)) < 0)
                {
                    perror(C_ERROR"1A-Shell");
                    exit(-1);
                }
                if((stdout_cpy = dup(STDOUT_FILENO)) < 0)
                {
                    perror(C_ERROR"okA-Shell");
                    exit(-1);
                }
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

            int pipes[2*num_pipes];
            for(int j = 0; j < num_pipes; j++)
            {
                if(pipe(pipes + 2*j) < 0)
                {
                    perror(C_ERROR"4A-Shell");
                    exit(1);
                }
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

            // for(int j = 0; j < n_cmmds; j++)
            // {
            //     if(j != 0)
            //     {
            //         if(dup2(pipes[(j-1)*2], STDIN_FILENO) < 0)
            //         {
            //             perror(C_ERROR"A-Shell: pipes");
            //             exit(-1);
            //         }
            //     }
            //     if(j != n_cmmds-1)
            //     {
            //         if(dup2(pipes[j*2+1], STDOUT_FILENO) < 0)
            //         {
            //             perror(C_ERROR"6A-Shell");
            //             exit(-1);
            //         }
            //     }
            //     else
            //     {
            //         if(dup2(stdout_master, STDOUT_FILENO) < 0)
            //         {
            //             perror(C_ERROR"6A-Shell");
            //             exit(-1);
            //         }
            //     }
            //     stdin_cpy = dup(STDIN_FILENO);
            //     stdout_cpy = dup(STDOUT_FILENO);
            //     execute_cmd(cmmds[j]);
            // }
            // dup2(stdout_master, STDOUT_FILENO);
            for(int j = 0; j < n_cmmds-1; j++)
            {
                int fork_ = fork();
                if(fork_ == 0)
                {
                    int pid = getpid();
                    setpgid(pid, pid);
                    if(j != 0)
                    {
                        if(dup2(pipes[(j-1)*2], STDIN_FILENO) < 0)
                        {
                            perror(C_ERROR"5A-Shell");
                            exit(-1);
                        }
                    }
                    stdin_cpy = dup(STDIN_FILENO);
                    if(dup2(pipes[j*2+1], STDOUT_FILENO) < 0)
                    {
                        perror(C_ERROR"6A-Shell");
                        exit(-1);
                    }
                    stdout_cpy = dup(STDOUT_FILENO);
                    execute_cmd(cmmds[j]);
                    exit(2);
                }
                else
                {
                    int pid = getpid();
                    setpgid(pid, pid);
                    int w;
                    waitpid(fork_, &w, WUNTRACED);
                }
            }
            int fork_ = fork();
            if(fork_ == 0)
            {
                int pid = getpid();
                setpgid(pid, pid);
                if(dup2(pipes[2*(num_pipes-1)], STDIN_FILENO) < 0)
                {
                    perror(C_ERROR"6A-Shell");
                    exit(-1);
                }
                if(dup2(stdout_master, STDOUT_FILENO) < 0)
                {
                    perror(C_ERROR"6A-Shell");
                    exit(-1);
                }
                stdin_cpy = dup(STDIN_FILENO);
                stdout_cpy = dup(STDOUT_FILENO);
                execute_cmd(cmmds[n_cmmds-1]);
                exit(2);
            }
            else
            {
                int pid = getpid();
                setpgid(pid, pid);
                int w;
                waitpid(fork_, &w, WUNTRACED);
            }
            for(int j = 0; j < 2*num_pipes; j++)
            {
                close(pipes[j]);
            }
        }
        free(inp);
        free(list);
        prompt();
    }
}