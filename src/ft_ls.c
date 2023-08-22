////////////////////////////////////////////////////////////////////////////////
// _ft_ls.c
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <dirent.h>
#include <sys/dirent.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <stdbool.h>
#include <unistd.h>

#include "../inc/ft_ls.h"
#include "../inc/file.h"

struct column_info {
	bool	valid;
	size_t	line_len;
	size_t	*col_arr;
};

static struct column_info *column_info;

static void
init_colum_info(size_t max_idx, size_t total_files)
{
	size_t i;
	size_t max_cols = max_idx < total_files ? max_idx : total_files;

	static size_t column_info_alloc;

	if (column_info_alloc < max_cols) {
		size_t new_column_info_alloc;
		size_t *p;

		if (max_cols < max_idx / 2) {
			column_info = realloc(column_info, max_cols * 2 * sizeof *column_info);
			new_column_info_alloc = max_cols * 2;
		}
		else {
			column_info = realloc(column_info, max_idx * sizeof *column_info);
			new_column_info_alloc = max_idx;
		}

		{
			size_t column_info_growth = new_column_info_alloc - column_info_alloc;
			size_t s = column_info_alloc + 1 + new_column_info_alloc;
			size_t t = s * column_info_growth;
			p = malloc(t / 2 * sizeof *p);
		}

		for (i = column_info_alloc; i < new_column_info_alloc; i++) {
			column_info[i].col_arr = p;
			p += i + 1;
		}

		column_info_alloc = new_column_info_alloc;
	}

	for (i = 0; i < max_cols; i++) {
		size_t j;

		column_info[i].valid = true;
		column_info[i].line_len = (i + 1) * 3;
		for (j = 0; j <= i; j++) {
			column_info[i].col_arr[j] = 3;
		}
	}
}


typedef struct s_directory
{
    char *name;
    t_file *files;
    struct s_directory *next;
} t_directory;

void
dir_add_back(t_directory **dir, char *dir_name)
{
    t_directory *temp = *dir;
    
    while (temp->next)
        temp = temp->next;
    temp->next = ft_calloc(sizeof(t_directory), 1);
    temp->next->name = dir_name;
}

int
open_directory(char *dir_name)
{
    struct dirent *rdir;
    t_directory *temp;
    DIR *dir;

    dir = opendir(dir_name);
    if (!dir) {
        write(2, "ft_ls: ", 7);
        perror(dir_name);
        return 1;
    }
    temp = calloc(sizeof(t_directory), 1);
    if (!temp)
        return 1;
    temp->name = dir_name;
    while ((rdir = readdir(dir))) {
        add_lexicographical(&(temp->files), rdir);
        if (rdir->d_type & DT_DIR && ft_strncmp("..", rdir->d_name, ft_strlen(rdir->d_name)) < 0) {
            char *_temp = ft_strjoin(dir_name, "/");
            dir_add_back(&temp, ft_strjoin(_temp, rdir->d_name));
            free(_temp);
        }
    }
    closedir(dir);
    for (t_file *i = temp->files; i; i = i->next)
        ft_printf("%s\n", i->rdir->d_name);
    temp = temp->next;
    for (; temp; temp = temp->next) {
        ft_printf("%s\n", temp->name);
        open_directory(temp->name);
    }
    return 0;
}

int
main(int ac, char **av)
{
    if (ac > 1) {
        if (open_directory(ft_strdup(av[1])))
            return 1;
    }
    else {
        if (open_directory(ft_strdup(".")))
            return 1;
    }
    init_colum_info(0, 0);
	return 0;
}
