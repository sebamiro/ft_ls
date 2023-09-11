#include <stdlib.h>
#include <stdio.h>
#include <sys/_types/_ssize_t.h>
#include <unistd.h>
#include <stdbool.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <sys/acl.h>
#include <sys/xattr.h>

#include "../inc/ft_ls.h"

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
    struct stat stat;
    char acl;
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

    total = ft_strlen(dirname) + ft_strlen(name) + 2;
    cat = xmalloc(total);
    cat[0] = 0;
    ft_strlcat(cat, dirname, total);
    ft_strlcat(cat, "/", total);
    ft_strlcat(cat, name, total);
    return cat;
}

static void
queue_directory(const char *name)
{
    t_pending *new = xmalloc(sizeof(t_pending));
    new->name = ft_strdup(name);
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
    void *t;
    size_t n;
    size_t newn;

    if (sorted_files_alloc < cwd_n + cwd_n / 2)
    {
        free(sorted);
        sorted = xmalloc(cwd_n * 3 * sizeof *sorted);
        sorted_files_alloc = 3 * cwd_n;
    }
    initialize_sort();

    n = cwd_n;
    while (n > 1) {
        newn = 0;
        for (size_t i = 1; i <= n - 1; i++) {
            int cmp = ft_strncmp(
                        ((t_fileinfo **)sorted)[i - 1]->name,
                        ((t_fileinfo **)sorted)[i]->name,
                        0xFFFFFF
                        );
            if ((mode & REVERSE && cmp < 0) || (mode ^ REVERSE && cmp > 0)) {
               t = sorted[i - 1]; 
               sorted[i - 1] = sorted[i];
               sorted[i] = t;
               newn = i;
            }
        }
        n = newn;
    }
}

static void
print_ident_digit(int ident, int digit)
{
    static char str[5] = "%xd ";
    str[1] = ident + 0x30;
    ft_printf(str, digit);
}

static void
print_byte_size(struct stat *s, bool print)
{
    static long long n = 10;
    static int ident = 1;

    if (!s) {
        n = 10;
        ident = 1;
        return ;
    }
    if (s->st_size >= n) {
        n *= 10;
        ident += 1;
        print_byte_size(s, false);
    }
    print ? print_ident_digit(ident, s->st_size) : 0;
}

static void
print_n_link(struct stat *s, bool print)
{
    static long long n = 10;
    static int ident = 1;

    if (!s) {
        n = 10;
        ident = 1;
        return ;
    }
    if (s->st_nlink >= n) {
        n *= 10;
        ident += 1;
        print_byte_size(s, false);
    }
    print ? print_ident_digit(ident, s->st_nlink) : 0;
}

static void
has_acl(const char *absolute, t_fileinfo *f) {
    ssize_t xattr;
    acl_t acl = NULL;
    acl_entry_t dummy;

    xattr = listxattr(absolute, NULL, 0, XATTR_NOFOLLOW);
    acl = acl_get_link_np(absolute, ACL_TYPE_EXTENDED);
    if (acl && acl_get_entry(acl, ACL_FIRST_ENTRY, &dummy) == -1) {
        acl_free(acl);
        acl = NULL;
    }
    if (xattr > 0) {
        f->acl = '@';
    }
    else if (acl)
        f->acl = '+';
    else
        f->acl = ' ';
    acl_free(acl);
}

static int
register_file(const char *name, const char *dirname, enum file_type type)
{
    t_fileinfo *f;
    size_t block;

    block = 0;
    if (cwd_n == cwd_alloc) {
        cwd = xrealloc(cwd,
               2 * cwd_alloc * sizeof *cwd
                );
        cwd_alloc *= 2;
    }
    f = &cwd[cwd_n];
    ft_bzero(f, sizeof *f);
    f->type = type;
    f->name = ft_strdup(name);
    cwd_n++;
    if (mode & TIME || mode & LIST) {
        char *absolute;

        if (name[0] == '/' || dirname[0] == 0)
            absolute = (char *)name;
        else
            absolute = file_name_concat(dirname, name);
        stat(absolute, &f->stat);
        has_acl(absolute, f);
        print_byte_size(&f->stat, false);
        print_n_link(&f->stat, false);
        block += f->stat.st_blocks;
        free(absolute);
    }
    return block;
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
print_owner(struct stat *s)
{
    char *owner;

    owner = getpwuid(s->st_uid)->pw_name;
    write(1, owner, ft_strlen(owner));
    write(1, "  ", 2);
}

static void
print_group(struct stat *s)
{
    char *group;

    group = getgrgid(s->st_gid)->gr_name;
    write(1, group, ft_strlen(group));
    write(1, "  ", 2);
}

static void
print_file_rights(t_fileinfo *f)
{
    char m[3] = "rwx";
    int x = S_IRUSR;
    for (int i = 0; i < 9; i++) {
        if (f->stat.st_mode & x)
            write(1, &m[i % 3], 1);
        else
            write(1, "-", 1);
        x >>= 1;
    }
    write(1, &f->acl, 1);
}

static void
print_long_format(t_fileinfo *f)
{
    write(1, f->type == directory ? "d" : "-", 1);
    print_file_rights(f);
    write(1, " ", 1);
    print_n_link(&f->stat, true);
    print_owner(&f->stat);
    print_group(&f->stat);
    print_byte_size(&f->stat, true);
    write(1, ctime((time_t *)&f->stat.st_mtimespec) + 4, 12);
    write(1, " ", 1);
    write(1, f->name, ft_strlen(f->name));
    write(1, "\n", 1);
}

static void
print_one_per_line(t_fileinfo *f)
{
    write(1, f->name, ft_strlen(f->name));
    write(1, "\n", 1);
}

static void
print_current_files(void)
{
    size_t i;

    for (i = 0; i < cwd_n; i++) {
        if (mode & LIST)
            print_long_format(((t_fileinfo **)sorted)[i]);
        else
            print_one_per_line(((t_fileinfo **)sorted)[i]);
    }
}

void
open_directory(const char *dir_name)
{
    DIR *dir;
    struct dirent *rdir;
    size_t block;

    block = 0;
    dir = opendir(dir_name);
    if (!dir) {
        write(2, "ft_ls: ", 7);
        perror(dir_name);
        return ;
    }

    clear_file();
    print_byte_size(NULL, false);
    print_n_link(NULL, false);

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
            block += register_file(rdir->d_name, dir_name, type);
        }
        else
            break;
    }
    closedir(dir);
    sort_files();
    if (mode & RECURSIVE)
        extract_dir_from_files(dir_name);
    if (cwd_n) {
        if (mode & LIST)
            ft_printf("total %d\n", (int)block);
        print_current_files();
    }
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
    cwd = xmalloc(cwd_alloc * sizeof *cwd);
    cwd_n = 0;

    open_directory(av[1] ? av[1] : ".");
    while (pending) {
        pending_dir = pending;
        write(1, "\n", 1);
        write(1, pending_dir->name, ft_strlen(pending_dir->name));
        write(1, ":\n", 2);
        pending = pending->next;
        open_directory(pending_dir->name);
        free(pending_dir->name);
        free(pending_dir);
    }
}
