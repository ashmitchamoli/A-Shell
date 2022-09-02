#include "headers.h"
#include "prompt.h"
#include "init_shell.h"
#include "input.h"
#include "execute.h"

extern int num_cmmds;

int main()
{
    init_shell();
    prompt();

    while(1)
    {
        char* inp = get_input();
        char** list = get_commands(inp);
        if(list == NULL)
        {
            prompt();
            continue;
        }
        for(int i = 0; i < num_cmmds; i++)
        {
            execute_cmd(list[i]);
        }
        free(inp);
        free(list);
        prompt();
    }
}