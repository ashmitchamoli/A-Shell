#include "headers.h"
#include "jobs.h"
#include "process_list.h"

extern p_list* head;

int cmp_jobs(const void* a, const void* b)
{
    char* x = (*((p_list**) a))->command;
    char* y = (*((p_list**) b))->command;

    return strcasecmp(x, y);
}

void A_Shell_jobs(int flags)
{
    // flag = 1 -> no flag, 2 -> -r, 3 -> -s, 6 -> -rs
    p_list* list[MAX_JOBS];
    int num_jobs = 0;
    for(p_list* i = head->next; i != NULL; i= i->next)
    {
        if(i->pid != -1)
        {
            list[num_jobs++] = i;
        }
    }
    qsort(list, num_jobs, sizeof(p_list*), cmp_jobs);

    for(int i = 0; i < num_jobs; i++)
    {
        if(list[i]->status == 'T' && flags != 2)
            printf("[%d] Stopped %s [%d]\n",  list[i]->index, list[i]->command, list[i]->pid);
        if(list[i]->status == 'R' && flags != 3)
            printf("[%d] Running %s [%d]\n", list[i]->index, list[i]->command, list[i]->pid);
        // printf("[%d] %c %s [%d]\n",  list[i]->index, state, list[i]->command, list[i]->pid);        
    }
}

void A_Shell_sig(int job_index, int signal_)
{
    for(p_list* i = head->next;  i !=  NULL; i=i->next)
    {
        if(i->index == job_index)
        {
            if(i->pid == -1)
            {
                fprintf(stderr, C_ERROR"A-Shell: sig: no job with index [%d] exists\n", job_index);
                fflush(NULL);
                return;
            }
            kill(i->pid, signal_);
            if(signal_ == 19)
            {
                i->status = 'T';
            }
            return;
        }
    }
    fprintf(stderr, C_ERROR"A-Shell: sig: no job with index [%d] exists\n", job_index);
    fflush(NULL);            
}

void A_Shell_fg(int job_index)
{
    for(p_list* i = head->next;  i !=  NULL; i=i->next)
    {
        if(i->index == job_index)
        {
            if(i->pid == -1)
            {
                fprintf(stderr, C_ERROR"A-Shell: fg: no job with index [%d] exists\n", job_index);
                fflush(NULL);
                return;
            }
            tcsetpgrp(STDIN_FILENO, i->pid);
            if(i->status == 'T')
            {
                kill(i->pid, SIGCONT);
            }
            int w;
            waitpid(i->pid, &w, WUNTRACED);
            i->pid = -1;
            free(i->command);
            i->command = NULL;
            i->status = 'Z';
            tcsetpgrp(STDOUT_FILENO, getpid());
            
            return;
        }
    }
    fprintf(stderr, C_ERROR"A-Shell: fg: no job with index [%d] exists\n", job_index);
    fflush(NULL);
}

void A_Shell_bg(int job_index)
{
    for(p_list* i = head->next;  i !=  NULL; i=i->next)
    {
        if(i->index == job_index)
        {
            if(i->pid == -1)
            {
                fprintf(stderr, C_ERROR"A-Shell: fg: no job with index [%d] exists\n", job_index);
                fflush(NULL);
                return;
            }
            if(i->status == 'T')
            {
                i->status = 'R';
                kill(i->pid, SIGCONT);
            }
            return;
        }
    }
    fprintf(stderr, C_ERROR"A-Shell: fg: no job with index [%d] exists\n", job_index);
    fflush(NULL);
}