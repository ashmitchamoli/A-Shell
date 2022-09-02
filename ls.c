#include "ls.h"
#include "headers.h"

extern char INIT_DIR[DIR_NAME_MAX];
extern int S_INIT_DIR;

int max(int a, int b)
{
    return a > b ? a : b;
}

int alphacmp(const void* a, const void* b)
{
    char* x = *((char**) a);
    char* y = *((char**) b);

    while(*x == '.' || *x == '/'|| *x == '~') x++;
    while(*y == '.' || *y == '/'|| *y == '~') y++;

    return strcasecmp(x, y);
}

int alphacmp_sdir(const struct dirent **a, const struct dirent **b)
{
    const char* x = (*a)->d_name;
    const char* y = (*b)->d_name;

    while(*x == '.' || *x == '/'|| *x == '~') x++;
    while(*y == '.' || *y == '/'|| *y == '~') y++;

    return strcasecmp(x, y);
}

int list_dir(char *dir, char* cur_dir, int flags)
{
    char months[][4] = {"Jan\0", "Feb\0", "Mar\0", "Apr\0", "May\0", "Jun\0", "Jul\0", "Aug\0", "Sep\0", "Nov\0", "Dec\0"};
    struct stat stats;
    if(dir[0] == '~')
    {
        cur_dir[strlen(cur_dir)] = '\0';
        if(stat(cur_dir, &stats) == -1)
        {
            perror(C_ERROR "A-Shell: ls");
            return -1;
        }
        cur_dir[strlen(cur_dir)] = '/';
    }
    else
    {
        if(stat(dir, &stats) == -1)
        {
            perror(C_ERROR "A-Shell: ls");
            return -1;
        }
    }
    if(!S_ISDIR(stats.st_mode))
    {
        mode_t perm = stats.st_mode;
        if(flags%2 == 0)
        {
            switch (perm & S_IFMT) {
                case S_IFBLK:   printf("b");        break;
                case S_IFCHR:   printf("c");        break;
                case S_IFDIR:   printf("d");        break;
                case S_IFIFO:   printf("p");        break;
                case S_IFLNK:   printf("l");        break;
                case S_IFREG:   printf("-");        break;
                case S_IFSOCK:  printf("s");        break;
                default:        printf("?");        break;
            }
            printf("%c", perm & S_IRUSR ? 'r' : '-');
            printf("%c", perm & S_IWUSR ? 'w' : '-');
            printf("%c", perm & S_IXUSR ? 'x' : '-');
            printf("%c", perm & S_IRGRP ? 'r' : '-');
            printf("%c", perm & S_IWGRP ? 'w' : '-');
            printf("%c", perm & S_IXGRP ? 'x' : '-');
            printf("%c", perm & S_IROTH ? 'r' : '-');
            printf("%c", perm & S_IWOTH ? 'w' : '-');
            printf("%c ", perm & S_IXOTH ? 'x' : '-');
            
            printf("%ld ", stats.st_nlink); 
 
            struct tm* date;
            struct passwd* u_name;
            struct group* g_name;
            
            u_name = getpwuid(stats.st_uid);
            g_name = getgrgid(stats.st_gid);
            printf("%s %s ", u_name->pw_name, g_name->gr_name);

            printf("%ld ", stats.st_size);

            date = localtime(&(stats.st_mtime));
            printf("%s %02d %d:%d ", months[date->tm_mon], date->tm_mday, date->tm_hour, date->tm_min);
        }
        
        if((perm & S_IXUSR) || (perm & S_IXGRP) || (perm & S_IXOTH))
        {
            printf(C_EXEC);
        }
        else
        {
            printf(C_FILE);
        }
        printf("%s\n", dir);
        printRESET();

        return 0;
    }

    struct dirent **list;
    int n = scandir(cur_dir, &list, NULL, alphacmp_sdir);
    if(n == -1)
    {
        if(errno == ENOTDIR)
        {
            printf(C_ERROR "A-Shell: ls: cannot access '%s'", dir);
        }
        else
        {
            printf(C_ERROR "A-Shell: ls: %s", dir);
        }
        fflush(stdout);
        perror(C_ERROR);
        printRESET();
        return -1;
    }

    int nlink_length = 0, size_length = 0, user_size = 0, size_grp = 0;
    for(int i = 0; i < n; i++)
    {
        char path[DIR_NAME_MAX] = {'\0'};
        strcpy(path, cur_dir);
        strcpy(path + strlen(cur_dir), list[i]->d_name);

        struct stat sb;
        if(stat(path, &sb) == -1)
        {
            printf(C_ERROR "A-Shell: ls: cannot access '%s'", dir);
            fflush(stdout);
            perror(C_ERROR);
            printRESET();
            return -1;
        }
        int t_nlink_length = 1, t_size_length = 1;
        long long t_nlink = sb.st_nlink, t_size = sb.st_size; 
        while(t_nlink/10 > 0)
        {
            t_nlink_length++;
            t_nlink /= 10;
        }
        while(t_size/10 > 0)
        {
            t_size_length++;
            t_size /= 10;
        }
        nlink_length = max(nlink_length, t_nlink_length);
        size_length = max(size_length, t_size_length);
        
        struct passwd* u_name;
        struct group* g_name;
        
        u_name = getpwuid(sb.st_uid);
        g_name = getgrgid(sb.st_gid);

        user_size = max(user_size, strlen(u_name->pw_name));
        size_grp = max(size_grp, strlen(g_name->gr_name));
    }
    for(int i = 0; i < n; i++)
    {
        if(flags%3 != 0 && (list[i]->d_name)[0] == '.')
        {
            continue;
        }
        char path[DIR_NAME_MAX] = {'\0'};
        strcpy(path, cur_dir);
        strcpy(path + strlen(cur_dir), list[i]->d_name);

        struct stat sb;
        // printf("%s\n", cur_dir);
        // fflush(stdout);
        if(stat(path, &sb) == -1)
        {
            printf(C_ERROR "3A-Shell: ls: cannot access '%s'", dir);
            fflush(stdout);
            perror(C_ERROR);
            printRESET();
            return -1;
        }

        mode_t perm = sb.st_mode;
        if(flags%2 == 0)
        {
            switch (sb.st_mode & S_IFMT) {
                case S_IFBLK:   printf("b");        break;
                case S_IFCHR:   printf("c");        break;
                case S_IFDIR:   printf("d");        break;
                case S_IFIFO:   printf("p");        break;
                case S_IFLNK:   printf("l");        break;
                case S_IFREG:   printf("-");        break;
                case S_IFSOCK:  printf("s");        break;
                default:        printf("?");        break;
            }
            printf("%c", perm & S_IRUSR ? 'r' : '-');
            printf("%c", perm & S_IWUSR ? 'w' : '-');
            printf("%c", perm & S_IXUSR ? 'x' : '-');
            printf("%c", perm & S_IRGRP ? 'r' : '-');
            printf("%c", perm & S_IWGRP ? 'w' : '-');
            printf("%c", perm & S_IXGRP ? 'x' : '-');
            printf("%c", perm & S_IROTH ? 'r' : '-');
            printf("%c", perm & S_IWOTH ? 'w' : '-');
            printf("%c ", perm & S_IXOTH ? 'x' : '-');
            printf("%*ld ", nlink_length, sb.st_nlink); 
 
            struct tm* date;
            struct passwd* u_name;
            struct group* g_name;
            
            u_name = getpwuid(sb.st_uid);
            g_name = getgrgid(sb.st_gid);
            printf("%*s %*s ",size_length, u_name->pw_name, size_grp, g_name->gr_name);

            printf("%*ld ", size_length, sb.st_size);

            date = localtime(&(sb.st_mtime));
            printf("%s %02d %02d:%02d ", months[date->tm_mon], date->tm_mday, date->tm_hour, date->tm_min);
        }
        if(S_ISDIR(perm))
        {
            printf(C_DIR);
        }
        else if(S_ISREG(perm))
        {
            if((perm & S_IXUSR) || (perm & S_IXGRP) || (perm & S_IXOTH))
            {
                printf(C_EXEC);
            }
            else
            {
                printf(C_FILE);
            }
        }
        printf("%s", list[i]->d_name);
        printRESET();

        printf("\n");
        free(list[i]);
    }
    free(list);
}

void A_Shell_ls(char** dirs, int num_dir, int flags)
{
    if(num_dir == 0 )
    {
        char temp[3] = {'\0'};
        temp[0] = '.';
        temp[1] = '/';
        list_dir(temp, temp, flags);
    }
    else
    {
        int num_files = 0, num_dirs = 0, num_invalid = 0;
        char** l_files = (char**) malloc(sizeof(char*) * num_dir);
        char** l_dirs = (char**) malloc(sizeof(char*) * num_dir);
        char** l_invalid = (char**) malloc(sizeof(char*) * num_dir);
        
        for(int i = 0; i < num_dir; i++)
        {
            char cur_dir[DIR_NAME_MAX] = {'\0'};
            if(dirs[i][0] == '~')
            {
                strcpy(cur_dir, INIT_DIR);
                int n = strlen(dirs[i]);
                if(n != 1)
                {
                    strcpy(cur_dir + S_INIT_DIR, dirs[i]+1);
                }
                // should I put a '/' at the end in this case
                // no because its already there
                // printf("%s\n", cur_dir);
                // fflush(stdout);
            }
            else
            {
                strcpy(cur_dir, dirs[i]);
                cur_dir[strlen(cur_dir)] = '/';
            }
            struct stat stats;
            if(dirs[i][0] == '~')
            {
                cur_dir[strlen(cur_dir)] = '\0';
                if(stat(cur_dir, &stats) == -1)
                {
                    l_invalid[num_invalid++] = dirs[i];
                    printf(C_ERROR"A-Shell: ls: cannot access '%s'", dirs[i]);
                    fflush(stdout);
                    perror(C_ERROR);        
                    printRESET();
                    continue;
                }
                cur_dir[strlen(cur_dir)] = '/';
            }
            else
            {
                if(stat(dirs[i], &stats) == -1)
                {
                    l_invalid[num_invalid++] = dirs[i];
                    printf(C_ERROR"A-Shell: ls: cannot access '%s'", dirs[i]);
                    fflush(stdout);
                    perror(C_ERROR);        
                    printRESET();
                    continue;
                }
            }
            if(S_ISDIR(stats.st_mode))
            {
                l_dirs[num_dirs++] = dirs[i];
            }
            else
            {
                l_files[num_files++] = dirs[i];
            }
        }
        qsort(l_files, num_files, sizeof(char**), alphacmp);
        qsort(l_dirs, num_dirs, sizeof(char**), alphacmp);
        for(int i = 0; i < num_files; i ++)
        {
            char cur_dir[DIR_NAME_MAX] = {'\0'};
            if(l_files[i][0] == '~')
            {
                strcpy(cur_dir, INIT_DIR);
                int n = strlen(l_files[i]);
                if(n != 1)
                {
                    strcpy(cur_dir + S_INIT_DIR, l_files[i]+1);
                }
                // should I put a '/' at the end in this case
                // no because its already there
                // printf("%s\n", cur_dir);
                // fflush(stdout);
            }
            else
            {
                strcpy(cur_dir, l_files[i]);
                cur_dir[strlen(cur_dir)] = '/';
            }
            list_dir(l_files[i], cur_dir, flags);
            if(i == num_files - 1)
            {
                printf("\n");
            }
        }
        for(int i = 0; i < num_dirs; i++)
        {
            char cur_dir[DIR_NAME_MAX] = {'\0'};
            if(l_dirs[i][0] == '~')
            {
                strcpy(cur_dir, INIT_DIR);
                int n = strlen(l_dirs[i]);
                if(n != 1)
                {
                    strcpy(cur_dir + S_INIT_DIR, l_dirs[i]+1);
                }
                // should I put a '/' at the end in this case
                // no because its already there
                // printf("%s\n", cur_dir);
                // fflush(stdout);
            }
            else
            {
                strcpy(cur_dir, l_dirs[i]);
                cur_dir[strlen(cur_dir)] = '/';
            }
            if(num_dirs != 1)
            {
                if(l_dirs[i][0] == '~')
                {
                    printGREEN();
                    printf("%s:\n", cur_dir);
                    printRESET();
                }
                else
                {
                    printGREEN();
                    printf("%s:\n", l_dirs[i]);
                    printRESET();
                }
            }
            // printf("%s\n", cur_dir);
            list_dir(l_dirs[i], cur_dir, flags);
            if(i != num_dirs-1)
            {
                printf("\n");
            }
        }
    }
}