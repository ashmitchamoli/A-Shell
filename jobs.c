#include "headers.h"
#include "jobs.h"
#include "process_list.h"

extern p_list* head;

void cmp_jobs(const void* a, const void* b)
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
        char path[DIR_NAME_MAX] = {'\0'};
        sprintf(path, "/proc/%d/stat", list[i]->pid);
        FILE* stat_f = fopen(path, "r");
        if(stat_f == NULL)
        {
            perror(C_ERROR "A-Shell: pinfo");
            printRESET();
            return;
        }
        int pid_; char comm[200]; char state;
        int ppid, pgrp, session_, tty_nr_, tpgid;

        fscanf(stat_f,"%d %s %c %d %d %d %d %d ", &pid_, comm, &state, &ppid, &pgrp, &session_, &tty_nr_, &tpgid);
        fclose(stat_f);
        
        if(state == 'R' && flags != 3)
            printf("[%d] Running %s [%d]\n", list[i]->index, list[i]->command, list[i]->pid);
        if(state == 'S' && flags != 2)
            printf("[%d] Stopped %s [%d]\n", list[i]->index, list[i]->command, list[i]->pid);
    }
}