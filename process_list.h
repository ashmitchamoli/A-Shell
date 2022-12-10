#ifndef __PROC_LIST_H
#define __PROC_LIST_H

typedef struct p_list
{
    struct p_list* next;
    char* command;
    char status;
    int pid;
    int index;
} p_list;

void add_list(char*, int, int);

#endif