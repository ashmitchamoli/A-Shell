#include "discover.h"
#include "headers.h"

extern char INIT_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;

void A_Shell_discover(char* dir, char* target, int flags)
{
    char cur_dir[DIR_NAME_MAX] = {'\0'};
    if(dir == NULL)
    {
        cur_dir[0] = '.';
        dir = cur_dir;
    }
    struct stat stat_dir;
    if(stat(dir, &stat_dir) == -1)
    {
        perror(C_ERROR "A-Shell: discover");
        return;
    }
    if(!S_ISDIR(stat_dir.st_mode))
    {
        printf(C_ERROR"A-Shell: discover: no such directory exists\n");
        printRESET();
        fflush(stdout);
        return;
    }
    struct dirent** list;
    int n = scandir(dir, &list, NULL, alphasort);
    if(n == -1)
    {
        printf(C_ERROR "A-Shell: discover: cannot access '%s'", dir);
        fflush(stdout);
        perror(C_ERROR);
        printRESET();
        return;
    }
    for(int i = 0; i < n; i++)
    {
        char path[DIR_NAME_MAX] = {'\0'};
        int __temp = strlen(dir);
        strcpy(path, dir);
        if(path[__temp-1] != '/')
            path[__temp++] = '/';
        strcpy(path + __temp, list[i]->d_name);

        struct stat dir_stats;
        if(stat(path, &dir_stats) == -1)
        {
            printf(C_ERROR "A-Shell: discover: cannot access '%s'", dir);
            fflush(stdout);
            perror(C_ERROR);
            printRESET();
            return;
        }
        if(!S_ISDIR(dir_stats.st_mode))
        {
            if((flags != 2) && (target == NULL || strcmp(target, list[i]->d_name) == 0))
            {
                if((dir_stats.st_mode & S_IXUSR) || (dir_stats.st_mode & S_IXGRP) || (dir_stats.st_mode & S_IXOTH))
                {
                    printf(C_EXEC);
                }
                else
                {
                    printf(C_FILE);
                }
                printf("%s\n", path);
                printRESET();
                fflush(stdout);
            }
        }
        else
        {
            if(strcmp(list[i]->d_name, ".") == 0 || strcmp(list[i]->d_name, "..") == 0)
            {
                continue;
            }
            if((flags != 3) && (target == NULL || strcmp(target, list[i]->d_name) == 0))
            {
                printf(C_DIR"%s\n", path);
                fflush(stdout);
                printRESET();
            }
            A_Shell_discover(path, target, flags);
        }
        free(list[i]);
    }
    free(list);
    return;
}