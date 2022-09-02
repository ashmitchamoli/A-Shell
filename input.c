#include "input.h"
#include "headers.h"

extern int num_cmmds;

char* get_input()
{
    char* inp = NULL;
    size_t alloc_inp; //size allocated to inp
    ssize_t s_inp = getline(&inp, &alloc_inp, stdin);
    if(s_inp == -1)
    {
        perror("\033[1;31mA-Shell");
        printRESET();
        exit(1);
    }
    if(inp[s_inp - 1] == '\n') inp[s_inp - 1] = '\0';
    return inp;
}

char** get_commands(char* inp)
{
    num_cmmds = 0; //to count the number of commands
    char* temp = strtok(inp, ";&");
    if(temp == NULL)
    {
        return NULL;
    }
    char** commands = (char**) malloc(sizeof(char*)*MAX_CMMDS);
    while(temp != NULL)
    {
        commands[num_cmmds++] = temp;
        temp = strtok(NULL, ";&");
    }
    return commands;
}