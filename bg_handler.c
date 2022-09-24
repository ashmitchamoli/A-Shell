#include "headers.h"
#include "bg_handler.h"
#include "process_list.h"
#include "prompt.h"

extern p_list* head;

void bg_handler()
{
    int wstatus;
    int pid = waitpid(-1, &wstatus, WUNTRACED | WNOHANG);
    p_list* proc = NULL;
    p_list* prev = head;
    for(p_list* i = head->next; i != NULL; i = i->next)
    {
        if(i->pid == pid)
        {
            proc = i;
            break;
        }
        prev = i;
    }
    if(proc == NULL)
    {
        return;
    }
    char* temp = (char*) malloc(sizeof(char) * strlen(proc->command));
    strcpy(temp, proc->command);
    temp = strtok(temp, " \t");
    if(WIFEXITED(wstatus))
    {
        fprintf(stderr ,"\n%s with pid %d exited normally\n", temp, pid);
    }
    else
    {
        fprintf(stderr ,"\n%s with pid %d exited abnormally\n", temp, pid);
    }
    free(proc->command);
    proc->command = NULL;
    proc->pid = -1;
    prompt();
    fflush(stderr);
    fflush(stdin);
}