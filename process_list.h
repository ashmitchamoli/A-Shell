#ifndef __PROC_LIST_H
#define __PROC_LIST_H

typedef struct p_list
{
    struct p_list* next;
    char* command;
    int pid;
    int index;
} p_list;

p_list add_list(char*, int, int);

#endif