#include "history.h"
#include "headers.h"

extern char USER[LOGIN_NAME_MAX];

void A_Shell_history()
{
    char path[DIR_NAME_MAX] = {'\0'};
    strcpy(path, "/home/");
    strcat(path, USER);
    strcat(path, "/.A_Shell_history.txt\0");
    FILE* f_hist = fopen(path, "r");
    if(f_hist == NULL)
    {
        perror(C_ERROR"A-Shell: error opening history file");
        printRESET();
        return;
    }
    char hist[MAX_HIST][MAX_CMMD_LEN];
    int num_ent = 0;
    while(fgets(hist[num_ent], MAX_CMMD_LEN, f_hist) != NULL)
    {
        num_ent++;
    }
    printWHITE(); printBOLD();
    for(int i = max(0, num_ent-10); i < num_ent; i++)
    {
        printf("%s", hist[i]);
    }
    printRESET();
    fclose(f_hist);
}