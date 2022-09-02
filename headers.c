#include "headers.h"

char USER[LOGIN_NAME_MAX];
char ROOT[HOST_NAME_MAX];
char INIT_DIR[DIR_NAME_MAX];
char PREV_DIR[DIR_NAME_MAX];
int S_INIT_DIR;
int num_cmmds;

void printBLACK()
{
    printf("\033[0;30m");
}
void printRED()
{
    printf("\033[0;31m");
}
void printGREEN()
{
    printf("\033[0;32m");
}
void printYELLOW()
{
    printf("\033[0;33m");
}
void printBLUE()
{
    printf("\033[0;34m");
}
void printPURPLE()
{
    printf("\033[0;35m");
}
void printCYAN()
{
    printf("\033[0;36m");
}
void printWHITE()
{
    printf("\033[0;37m");
}
void printBOLD()
{
    printf("\033[1m");
}
void printRESET()
{
    printf("\033[0m");
}