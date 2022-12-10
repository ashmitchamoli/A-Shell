#include "pinfo.h"
#include "headers.h"

void A_Shell_pinfo(int pid)
{
    printYELLOW(); printBOLD();
    char path[DIR_NAME_MAX] = {'\0'};
    sprintf(path, "/proc/%d/stat", pid);
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
    printf("pid : %d\n", pid);
    printf("process statts : %c%c\n", state, (tpgid == pgrp) ? '+' : ' ');
    char path_[DIR_NAME_MAX] = {'\0'};
    sprintf(path_, "/proc/%d/statm", pid);
    char buf[1024];
    FILE* f = fopen(path_, "r");
    long long man;
    fscanf(f, "%lld", &man);
    printf("memory : %lld {Virtual Memory}\n", man);
    fflush(stdout);
    fclose(f);
    fclose(stat_f);
    sprintf(path, "/proc/%d/exe", pid);
    for(int i = 0; i < 1024; i++) buf[i] = '\0';
    if(readlink(path, buf, 1024) == -1)
    {
        fprintf(stderr ,C_ERROR"A-Shell: pinfo: error reading executable link");
        fflush(stdout);
        perror(C_ERROR);
        printRESET();
        return;
    }
    printf("executable path : %s\n", buf);
    printRESET();
}