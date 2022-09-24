#include "headers.h"
#include "process_list.h"

p_list* head;

p_list add_list(char* command, int pid, int index)
{
    for(p_list* i = head->next; i != NULL; i=i->next)
    {
        if(i->pid == -1)
        {
            i->command = (char*) malloc(sizeof(char) * strlen(command));
            strcpy(i->command, command);
            i->pid = pid;
            return;
        }
    }
    p_list* new_p = (p_list*) malloc(sizeof(p_list));
    new_p->command = (char*) malloc(sizeof(char) * strlen(command));
    strcpy(new_p->command, command);
    new_p->index = index;
    new_p->pid = pid;
    new_p->next = head->next;
    head->next = new_p;
}