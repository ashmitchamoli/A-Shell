#ifndef __HEADERS_H
#define __HEADERS_H

#include <stdio.h>
#include <stdlib.h>
#include <pwd.h>
#include <grp.h>
#include <unistd.h>
#include <limits.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/utsname.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <dirent.h>
#include <time.h>
#include <ctype.h>

#define DIR_NAME_MAX 1024
#define MAX_CMMDS 1024
#define MAX_CMMD_LEN 1024
#define MAX_DIR_LS 1024
#define C_ERROR "\033[1;31m"
#define C_FILE "\033[0;32m"
#define C_EXEC "\033[1;32m"
#define C_DIR "\033[1;034m"
#define MAX_HIST 20
#define DEFAULT_HIST 10
#define MAX_ARGS 1024

void printRED();
void printBLACK();
void printGREEN();
void printYELLOW();
void printBLUE();
void printPURPLE();
void printCYAN();
void printWHITE();
void printBOLD();
void printRESET();
int min(int, int);
int max(int, int);

#endif