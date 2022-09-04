#include "pinfo.h"
#include "headers.h"

void A_Shell_pinfo(int pid)
{
    printYELLOW(); printBOLD();
    char path[DIR_NAME_MAX] = {'\0'};
    sprintf(path, "/proc/%d/status", pid);
    FILE* f = fopen(path, "r");
    if(f == NULL)
    {
        perror(C_ERROR "A-Shell: pinfo");
        printRESET();
        return;
    }
    printf("pid : %d\n", pid);
    char buf[1024];
    while(fgets(buf, 1024, f) != NULL)
    {
        if(strncmp(buf, "State", 5) == 0)
        {
            char* temp = strtok(buf, " \t");
            temp = strtok(NULL, " \t");
            printf("process status : %s", temp);
            temp = strtok(NULL, " \t\n");
            if(strcmp("(running)", temp) == 0)
            {
                printf("+");
            }
            printf("\n");
            continue;
        }
        if(strncmp(buf, "VmSize", 6) == 0)
        {
            char* temp = strtok(buf, " \t");
            temp = strtok(NULL, " \t");
            printf("memory : %s {Virtual Memory}", temp);
            printf("\n");
            break;
        }
    }
    fflush(stdout);
    fclose(f);
    sprintf(path, "/proc/%d/exe", pid);
    if(readlink(path, buf, 1024) == -1)
    {
        printf(C_ERROR"A-Shell: pinfo: error reading executable link");
        fflush(stdout);
        perror(C_ERROR);
        printRESET();
        return;
    }
    printf("executable path : %s\n", buf);
    printRESET();
}