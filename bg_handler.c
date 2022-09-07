#include "headers.h"
#include "bg_handler.h"
#include "process_list.h"
#include "prompt.h"

extern p_list* head;
extern int num_bg;

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
    char* temp = strtok(proc->command, " \t");
    if(WIFEXITED(wstatus))
    {
        fprintf(stderr ,"\n%s with pid %d exited normally\n", temp, pid);
    }
    else
    {
        fprintf(stderr ,"\n%s with pid %d exited abnormally\n", temp, pid);
    }
    prompt();
    fflush(stderr);
    fflush(stdin);
    num_bg--;
    prev->next = proc->next;
    free(proc);
}