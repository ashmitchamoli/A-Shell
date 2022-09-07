#include "headers.h"
#include "process_list.h"

p_list* head;

p_list add_list(char* command, int pid)
{
    p_list* new_p = (p_list*) malloc(sizeof(p_list));
    new_p->command = (char*) malloc(sizeof(char) * strlen(command));
    strcpy(new_p->command, command);
    new_p->pid = pid;
    new_p->next = head->next;
    head->next = new_p;
}