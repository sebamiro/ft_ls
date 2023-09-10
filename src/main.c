#include <stdlib.h>
#include <stdio.h>
#include <sys/dirent.h>
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

enum file_type {
    unknown,
    blockdev,
    chardev,
    directory,
    fifo,
    symbolic_link,
    normal,
    sock
};

typedef struct s_pending
{
    char *name;
    struct s_pending *next;
} t_pending;

typedef struct s_fileinfo
{
    char *name;
    enum file_type type;
} t_fileinfo;

static char mode = 0;
t_pending *pending = NULL;
t_fileinfo *cwd;
void **sorted;
size_t cwd_n;
size_t cwd_alloc;
size_t sorted_files_alloc;

static void
clear_file(void)
{
    size_t i;

    for (i = 0; i < cwd_n; i++) {
        t_fileinfo *f = sorted[i];
        free(f->name);
    }
    cwd_n = 0;
}

static char*
file_name_concat(const char *dirname, const char *name)
{
    size_t total;
    char *cat; 

    total = strlen(dirname) + strlen(name) + 2;
    cat = calloc(total, 1);
    if (!cat)
        exit(3);
    strlcat(cat, dirname, total);
    strlcat(cat, "/", total);
    strlcat(cat, name, total);
    return cat;
}

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

static void
initialize_sort(void)
{
    size_t i;

    for (i = 0; i < cwd_n; i++) {
        sorted[i] = &cwd[i];
    }
}

static void
sort_files(void)
{
    if (sorted_files_alloc < cwd_n + cwd_n / 2)
    {
        free(sorted);
        sorted = malloc(cwd_n * 3 * sizeof *sorted);
        sorted_files_alloc = 3 * cwd_n;
    }
    initialize_sort();

    size_t n = cwd_n;
    while (n > 1) {
        size_t newn = 0;
        for (size_t i = 1; i <= n - 1; i++) {
            if (strcmp(
                        ((t_fileinfo **)sorted)[i - 1]->name,
                        ((t_fileinfo **)sorted)[i]->name) > 0
                    ) {
               void *t = sorted[i - 1]; 
               sorted[i - 1] = sorted[i];
               sorted[i] = t;
               newn = i;
            }
        }
        n = newn;
    }
}

static void
register_file(const char *name, enum file_type type)
{
    t_fileinfo *f;

    if (cwd_n == cwd_alloc) {
        cwd = realloc(cwd, cwd_alloc * 2 * sizeof *cwd);
        cwd_alloc *= 2;
    }
    f = &cwd[cwd_n];
    bzero(f, sizeof *f);
    f->type = type;
    f->name = strdup(name);
    if (!f->name)
        exit(3);
    cwd_n++;
}

static bool
dot_or_dotdot(const char *file_name)
{
    if (file_name[0] == '.')
        return !file_name[(file_name[1] == '.') + 1];
    return false;
}

static void
extract_dir_from_files(const char *dir_name)
{
    size_t i;

    for (i = cwd_n; i-- != 0;) {
        t_fileinfo *f = sorted[i];

        if (f->type == directory && !dot_or_dotdot(f->name)) {
            if (!dir_name || f->name[0] == '/')
                queue_directory(f->name);
            else {
                char *name = file_name_concat(dir_name, f->name);
                queue_directory(name);
                free(name);
            }
        }
    }
}

static void
print_current_files(void)
{
    size_t i;

    for (i = 0; i < cwd_n; i++) {
        write(1, ((t_fileinfo **)sorted)[i]->name,
                strlen(((t_fileinfo **)sorted)[i]->name));
        write(1, "\n", 1);
    }
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
        return ;
    }

    clear_file();

    while (1) {
        rdir = readdir(dir);
        if (rdir) {
            enum file_type type = unknown;

            if (~mode & ALL && rdir->d_name[0] == '.')
                continue ;
            switch (rdir->d_type) {
                case DT_BLK: type = blockdev; break;
                case DT_CHR: type = chardev; break;
                case DT_DIR: type = directory; break;
                case DT_FIFO: type = fifo; break;
                case DT_LNK: type = symbolic_link; break;
                case DT_REG: type = normal; break;
                case DT_SOCK: type = sock; break;
            }
            register_file(rdir->d_name, type);
        }
        else
            break;
    }
    closedir(dir);
    sort_files();
    if (mode & RECURSIVE)
        extract_dir_from_files(dir_name);
    if (cwd_n)
        print_current_files();
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

    cwd_alloc = 100;
    cwd = malloc(cwd_alloc * sizeof *cwd);
    if (!cwd)
        exit(3);
    cwd_n = 0;

    open_directory(av[1] ? av[1] : ".");
    while (pending) {
        pending_dir = pending;
        write(1, "\n", 1);
        write(1, pending_dir->name, strlen(pending_dir->name));
        write(1, ":\n", 2);
        pending = pending->next;
        open_directory(pending_dir->name);
        free(pending_dir->name);
        free(pending_dir);
    }
}
