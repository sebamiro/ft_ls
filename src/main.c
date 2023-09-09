#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>

enum modes {
    LIST = 0x01,
    RECURSIVE = 0x02,
    ALL = 0x04,
    REVERSE = 0x08,
    TIME = 0x10
};

typedef struct s_pending
{
    char *name;
    struct s_pending *next;
} t_pending;

static char mode = 0;
t_pending *pending = NULL;

static void
queue_directory(const char *name)
{
    t_pending *new = malloc(sizeof(t_pending));
    if (!new)
        exit(3);
    new->name = strdup(name);
    if (!new->name)
        exit(3);
    new->next = pending;
    pending = new;
}

void
open_directory(const char *dir_name)
{
    DIR *dir;
    struct dirent *rdir;

    dir = opendir(dir_name);
    if (!dir) {
        write(2, "ft_ls: ", 7);
        perror(dir_name);
        exit(1);
    }
    while ((rdir = readdir(dir))) {
        if (~mode & ALL && rdir->d_name[0] == '.')
            continue ;
        write(1, rdir->d_name, strlen(rdir->d_name));
        write(1, "\n", 1);
        if (mode & RECURSIVE && rdir->d_type & DT_DIR
                && strcmp("..", rdir->d_name) < 0) {
            char temp[4096] = {0};
            strlcat(temp, dir_name, sizeof(temp));
            strlcat(temp, "/", sizeof(temp));
            strlcat(temp, rdir->d_name, sizeof(temp));
            queue_directory(temp);
        }
    }
    closedir(dir);
}

static bool
_getopt(char *opt)
{
    if (opt[0] != '-')
        return false;
    for (int i = 1; opt[i]; i++) {
        switch (opt[i]) {
            case 'l':
                mode |= LIST; break;
            case 'R':
                mode |= RECURSIVE; break;
            case 'a':
                mode |= ALL; break;
            case 'r':
                mode |= REVERSE; break;
            case 't':
                mode |= TIME; break;
            default:
                write(2, "ft_ls: invalid option -- '", 27); 
                write(2, &opt[i], 1); 
                write(2, "'\n", 2); 
                exit(2);
        }
    }
    return true;
}

int
main(int ac, char **av)
{
    t_pending *pending_dir;

    if (ac > 1 && _getopt(av[1]))
        av = &av[1];
    open_directory(av[1] ? av[1] : ".");
    pending_dir = pending ? pending : NULL;
    while (pending_dir) {
        write(1, "\n", 1);
        write(1, pending_dir->name, strlen(pending_dir->name));
        write(1, ":\n", 2);
        pending = pending->next;
        open_directory(pending_dir->name);
        free(pending_dir->name);
        free(pending_dir);
        pending_dir = pending;
    }
}
