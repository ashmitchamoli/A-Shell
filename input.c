#include "input.h"
#include "headers.h"
#include "process_list.h"
#include "bg_handler.h"
#include "prompt.h"

extern int num_cmmds;
extern char USER[LOGIN_NAME_MAX];
extern p_list* head;
extern int num_bg;
extern char INIT_DIR[DIR_NAME_MAX];

struct termios orig_termios;

void disableRawMode() {
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == -1)
    {
        perror("A-Shell");
        exit(1);
    }
}

void enableRawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1)
    {
        perror("A-Shell");
        exit(1);
    }
    atexit(disableRawMode);
    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1)
    {
        perror("A-Shell");
        exit(1);
    }
}


int bg_create(char* cmd)
{
    char* temp = strtok(cmd, " \t");
    if(temp == NULL)
    {
        return -1;
    }
    char* args[MAX_ARGS];
    int _num_args = 1;
    args[0] = temp;
    temp = strtok(NULL, " \t");
    while(temp != NULL)
    {
        args[_num_args++] = temp;
        temp = strtok(NULL, " \t");
    }

    int fork_id = fork();

    if(fork_id == 0)
    {
        int pid = getpid();
        setpgid(pid, pid);
        signal (SIGINT, SIG_DFL);
        signal (SIGTTIN, SIG_DFL);
        signal (SIGTTOU, SIG_DFL);
        args[_num_args] = NULL;
        execvp(args[0], args);
        fprintf(stderr, C_ERROR "A-Shell: %s: command not found", args[0]);
        printRESET();
        fflush(stdout);
        perror(C_ERROR"");
        printRESET();
        exit(2);
    }
    else
    {
        signal (SIGINT, SIG_IGN);
        signal (SIGTTIN, SIG_IGN);
        signal (SIGTTOU, SIG_IGN);
        num_bg++;
        add_list(cmd, fork_id, num_bg);
        printCYAN();
        printf("[%d] %d\n", num_bg, fork_id);
        printRESET();
        fflush(stdout);
        int pid = getpid();
        setpgid(pid, pid);
        tcsetpgrp (STDIN_FILENO, pid);
        signal(SIGCHLD, bg_handler);
        return 0;
    }
}

void handle_history(char* inp)
{
    int isEmpty = 1;
    for(int i = 0; i < strlen(inp); i++)
    {
        if(inp[i] != ' ' && inp[i] != '\t')
        {
            isEmpty = 0;
            break;
        }
    }
    int __n = strlen(inp);
    inp[__n] = '\n';
    char hist[MAX_HIST][MAX_CMMD_LEN];
    int num_ent = 0;
    char path[DIR_NAME_MAX] = {'\0'};
    strcpy(path, "/home/");
    strcat(path, USER);
    strcat(path, "/.A_Shell_history.txt\0");
    FILE* f_hist = fopen(path, "r");
    if(f_hist == NULL)
    {
        perror(C_ERROR "A-Shell: cannot read history file");
        exit(1);
    }
    while(fgets(hist[num_ent], MAX_CMMD_LEN, f_hist) != NULL)
    {
        num_ent++;
    }
    fclose(f_hist);
    inp[__n + 1] == '\0';
    if(isEmpty || strcmp(hist[num_ent-1], inp) == 0)
    {
        inp[__n] = '\0';
        return;
    }
    f_hist = fopen(path, "w");
    if(num_ent > MAX_HIST)
    {
        fprintf(stderr ,C_ERROR "A-Shell: files tampered");
        fclose(f_hist);
        exit(1);
    }
    else if(num_ent == MAX_HIST)
    {
        int w;
        for(int i = 1; i < MAX_HIST; i++)
        {
            w = fputs(hist[i], f_hist);
            if(w == EOF)
            {
                perror(C_ERROR "A-Shell: error writing history file");
                fclose(f_hist);
                exit(1);
            }
        }
        inp[__n + 1] = '\0';
        w = fputs(inp, f_hist);
        if(w == EOF)
        {
            perror(C_ERROR "A-Shell: error writing history file");
            fclose(f_hist);
            exit(1);
        }
    }
    else
    {
        inp[__n + 1] = '\0';
        int w;
        for(int i = 0; i < num_ent; i++)
        {
            w = fputs(hist[i], f_hist);
            if(w == EOF)
            {
                perror(C_ERROR "A-Shell: error writing history file");
                fclose(f_hist);
                exit(1);
            }
        }
        inp[__n + 1] = '\0';
        w = fputs(inp, f_hist);
        if(w == EOF)
        {
            perror(C_ERROR "A-Shell: error writing history file");
            fclose(f_hist);
            exit(1);
        }
    }
    inp[__n] = '\0';
    fclose(f_hist);
}

int alphacmp_sdir_auto(const struct dirent **a, const struct dirent **b)
{
    const char* x = (*a)->d_name;
    const char* y = (*b)->d_name;
    const char* t1 = x;
    const char* t2 = y;
    
    while(*x == '.' || *x == '/'|| *x == '~' || *x == '\'' || *x == '\"') x++;
    while(*y == '.' || *y == '/'|| *y == '~' || *y == '\'' || *y == '\"') y++;

    if(strcasecmp(x, y) == 0)
    {
        return (strlen(t1) < strlen(t2) ? -1 : 1);
    }
    return strcasecmp(x, y);
}

void autocomplete(char* inp)
{
    if(strlen(inp) == 0 || inp[strlen(inp)-1] == ' ')
    {
        return;
    }
    char input[1024];
    strcpy(input, inp);
    char* temp = input + strlen(input) - 1;
    while(temp != input-1 && *temp != ' ')
    {
        temp--;
    }
    temp++;
    int s_temp = strlen(temp);
    struct dirent **list;
    char path[1024] = {'\0'};
    if(temp[0] == '~')
    {
        strcpy(path, INIT_DIR);
        path[strlen(path)] = '/';
        if(s_temp > 1)
        {
            strcat(path, temp+2);
        }
    }
    else
    {
        strcpy(path, temp);
    }
    int s_path = strlen(path);
    // if(path[s_path-1] == '/')
    // {
    //     temp[0] = '\0';
    // }
    int isDir = 0;
    for(int i = s_path-1; i > -1; i--)
    {
        if(path[i] == '/')
        {
            isDir = 1;
            path[i+1] = '\0';
            break;
        }
    }
    if(!isDir)
    {
        path[0] = '\0';
    }
    for(int i = s_temp-1; i > -1; i--)
    {
        if(temp[i] == '/')
        {
            temp = &(temp[i+1]);
            break;
        }
    }
    if(strlen(path) == 0)
    {
        path[0] = '.';
        path[1] = '/';
        path[2] = '\0';
    }
    int n = scandir(path, &list, NULL, alphacmp_sdir_auto);
    if(n == -1)
    {
        perror(C_ERROR"A-Shell"C_RESET);
        printRESET();
        return;
    }
    int s_key = strlen(temp);
    char* poss_names[1024]; int count_names = 0;
    for(int i = 0; i < n; i++)
    {
        if(strncmp(temp, list[i]->d_name, s_key) == 0)
        {
            poss_names[count_names++] = list[i]->d_name;
        }
    }
    if(count_names == 0)
    {
        for(int i = 0; i  < n; i++)
        {
            free(list[i]);
        }
        free(list);
        return;
    }
    else if(count_names == 1)
    {
        for(int i = 0; i < s_key; i++)
        {
            printf("\b \b");
        }
        printf("%s", poss_names[0]);
        int t = strlen(inp);
        for(int i = t - s_key; i < t; i++)
        {
            inp[i] = '\0';
        }
        strcat(inp, poss_names[0]);
        
        struct stat stats;
        char path_[1024] = {'\0'};
        strcpy(path_, path);
        strcat(path_, poss_names[0]);
        if(stat(path_, &stats) < 0)
        {

            perror("A-Shell: autocomplete");
            return;
        }
        if(!S_ISDIR(stats.st_mode))
        {
            inp[strlen(inp)] = ' ';
            printf(" ");
        }
        else
        {
            inp[strlen(inp)] = '/';
            printf("/");
        }
        for(int i = 0; i  < n; i++)
        {
            free(list[i]);
        }
        free(list);
        
        return;
    }
    char common[1024] = {'\0'};
    strcpy(common, poss_names[0]);
    for(int i = 1; i < count_names; i++)
    {
        int n__ = strlen(poss_names[i]);
        for(int j = 0; j < n__; j++)
        {
            if(common[j] != poss_names[i][j])
            {
                common[j] = '\0';
                break;
            }
        }
        if(common[0] == '\0')
        {
            break;
        }
    }
    if(common[0] != '\0')
    {
        int t = strlen(inp);
        for(int i = t - s_key; i < t; i++)
        {
            inp[i] = '\0';
        }
        strcat(inp, common);
    }
    printf("\n");
    for(int i = 0; i < count_names; i++)
    {
        printf("%s", poss_names[i]);
        struct stat stats;
        char path_[1024] = {'\0'};
        strcpy(path_, path);
        strcat(path_, poss_names[i]);
        if(stat(path_, &stats) < 0)
        {
            perror("A-Shell: autocomplete");
            return;
        }
        if(S_ISDIR(stats.st_mode))
        {
            printf("/");
        }
        printf("\n");
    }
    prompt();
    printf("%s", inp);
    for(int i = 0; i  < n; i++)
    {
        free(list[i]);
    }
    free(list);
    return;
}

char* get_input()
{
    char* inp = malloc(sizeof(char) * MAX_CMMD_LEN);
    memset(inp, '\0', MAX_CMMD_LEN);
    char c;

    setbuf(stdout, NULL);
    enableRawMode();
    int inp_size = 0;
    while(read(STDIN_FILENO, &c, 1) == 1)
    {
        if(iscntrl(c))
        {
            if(c == 10)
            {
                printf("\n");
                break;
            }
            else if(c == 4) // ctrl + D
            {
                printf("\n");
                exit(1);
            }
            else if(c == 127) // backspace
            {
                if(inp_size > 0)
                {
                    inp[--inp_size] = '\0';
                    printf("\b \b");
                }
            }
            else if(c == 9) // tab
            {
                // handle auto complete
                autocomplete(inp);
                inp_size = strlen(inp);
            }
            else if(c == 12) // ctrl + L
            {

            }
            else 
            {
                printf("%c", c);
            }
        }
        else
        {
            inp[inp_size++] = c;
            printf("%c", c);
        }
    }
    disableRawMode();

    // if(s_inp > MAX_CMMD_LEN - 1)
    // {
    //     fprintf(stderr ,C_ERROR "A-Shell: input length too long");
    //     return NULL;
    // }
    // if(inp[s_inp - 1] == '\n') inp[s_inp - 1] = '\0';
    return inp;
}

char** get_commands(char* inp)
{
    // history handling
    handle_history(inp);

    num_cmmds = 0; //to count the number of commands
    char* temp = strtok(inp, ";");
    if(temp == NULL)
    {
        return NULL;
    }
    char** commands = (char**) malloc(sizeof(char*)*MAX_CMMDS);
    while(temp != NULL)
    {
        commands[num_cmmds++] = temp;
        temp = strtok(NULL, ";");
    }
    char** shell_commands = (char**) malloc(sizeof(char*)*MAX_CMMDS);
    int num_shell_cmmds = 0;
    for(int i = 0; i < num_cmmds; i++)
    {
        char* prev = commands[i]; int n = strlen(commands[i]);
        for(int j = 0; j < n; j++)
        {
            if(commands[i][j] == '&')
            {
                commands[i][j] = '\0';
                bg_create(prev);
                prev = &(commands[i][j+1]);
            }
        }
        shell_commands[num_shell_cmmds++] = prev;
    }
    free(commands);
    num_cmmds = num_shell_cmmds;
    return shell_commands;
}