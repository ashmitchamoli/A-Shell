#include "history.h"
#include "headers.h"

void A_Shell_history()
{
    FILE* f_hist = fopen("/tmp/A-Shell_history.txt", "r");
    if(f_hist == NULL)
    {
        perror(C_ERROR"A-Shell: error opening '/tmp/A-Shell_history.txt'");
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