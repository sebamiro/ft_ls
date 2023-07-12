////////////////////////////////////////////////////////////////////////////////
// _ft_ls.c
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <dirent.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <sys/types.h>
#include <stdbool.h>

#include "ft_ls.h"
#include "file.h"

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

int
main(void)
{
	DIR *dir = opendir(".");
	if (!dir) {
		write(2, "ft_ls: ", 7);
		perror("nc");
		return 1;
	}
	struct dirent *rdir;
	t_file *file = NULL;
	size_t n = 0;
	while ((rdir = readdir(dir))) {
		add_lexicographical(&file, rdir);
		n++;
	}

	#ifdef TIOCGWINSZ
		int cols = 0;
		struct winsize ws;

		if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1 && 0 < ws.ws_col && ws.ws_col == (size_t) ws.ws_col)
			cols = ws.ws_col;
	#endif

	init_colum_info(cols / 3 + 1, n);

	for (t_file *i = file; i != NULL; i = i->next)
		ft_printf("%s\n", i->rdir->d_name);
	closedir(dir);
	return 0;
}
