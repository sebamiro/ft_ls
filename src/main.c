//
// smiro ft_ls
// main.c
// 2024-02
//

#include "../lib/libft/libft.h"
// #include "libft/libft.h"

#include "defines.h"

#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

static bool get_opt(const char *opt);
static void *xmalloc(size_t size);
static char *xstrdup(const char *s);
static int open_directroy(const char *dir_name);
static int open_pending(int error, pending_t *pending_dir);

int main(int ac, char **av)
{
	if (ac && get_opt(*(++av))) {
		av++;
	}
	cwd_alloc = 100;
	cwd = xmalloc(cwd_alloc * sizeof *cwd);
	cwd_n = 0;
	return open_pending(open_directroy(*av ? *av : "."), pending);
}

static void clear_files(void);
static file_type get_type(unsigned char);
static size_t register_file(const char *, const char *, file_type);
static void sort_files(void);
static void extract_dir(const char *dir_name);
static void print_current_files(void);

static int open_directroy(const char *dir_name)
{
	DIR	*dir;
	size_t block;
	struct dirent *rdir;

	if (!(dir = opendir(dir_name))) {
		write(2, "ft_ls: ", 7);
		perror(ft_strrchr(dir_name, '/') + 1);
		return 1;
	}
	block = 0;
	clear_files();
	while ((rdir = readdir(dir))) {
		if (~flags & ALL && rdir->d_name[0] == '.') {
			continue;
		}
		block += register_file(rdir->d_name, dir_name, get_type(rdir->d_type));
	}
	closedir(dir);
	sort_files();
	if (cwd_n) {
		if (flags & RECURSIVE) {
			extract_dir(dir_name);
		}
		if (flags & LIST) {
			//ft_printf("total %d\n", (int)block);
		}
		print_current_files();
	}
	return 0;
}

static void print_one_per_line(const char *file_name);

static void print_current_files(void)
{
	size_t i;

	for (i = 0; i < cwd_n; i++) {
		if (flags & LIST) {
			// TODO
		} else {
			print_one_per_line(((fileinfo_t **)sorted)[i]->name);
		}
	}
}

static void print_one_per_line(const char *file_name)
{
	write(1, file_name, ft_strlen(file_name));
	write(1, "\n", 1);
}

static bool dot_or_dotdot(const char *);
static void queue_directory(const char *name);
static void file_name_concat(char *dest, const char *dir, const char *file);

static void extract_dir(const char *dir_name)
{
	size_t i;
	fileinfo_t *f;
	char name[1024];

	for (i = cwd_n; i-- > 0;) {
		f = sorted[i];
		if (f->type == directory && !dot_or_dotdot(f->name)) {
			if (!dir_name || f->name[0] == '/') {
				queue_directory(f->name);
			} else {
				file_name_concat(name, dir_name, f->name);
				queue_directory(name);
			}
		}
	}
}

static void queue_directory(const char *name)
{
	pending_t *n;

	n = xmalloc(sizeof * pending);
	n->name = xstrdup(name);
	n->next = pending;
	pending = n;
}

static bool dot_or_dotdot(const char *file_name)
{
    if (file_name[0] == '.')
        return !file_name[(file_name[1] == '.') + 1];
    return false;
}


static void initialize_sort(void);

static void sort_files(void)
{
	void *t;
	size_t i, n, newn;
	long long cmp;

	if (sorted_alloc < cwd_n * 1.5) {
		sorted_alloc = cwd_n * 2;
		sorted = realloc(sorted, sorted_alloc * sizeof *sorted);
		if (!sorted) {
			exit(1);
		}
	}
	initialize_sort();
	n = cwd_n;
	while (n > 1) {
		newn = 0;
		for (i = 1; i < n; i++) {
			if (flags & TIME) {
				cmp = ((fileinfo_t **)sorted)[i]->stat.st_mtimespec.tv_sec
					- ((fileinfo_t **)sorted)[i - 1]->stat.st_mtimespec.tv_sec;
			} else {
				cmp = ft_strcmp(((fileinfo_t **)sorted)[i - 1]->name,
						((fileinfo_t **)sorted)[i]->name);
			}
			if (flags & REVERSE) {
				cmp = -cmp;
			}
			if (cmp > 0) {
				t = sorted[i - 1];
				sorted[i - 1] = sorted[i];
				sorted[i] = t;
				newn = i;
			}
		}
		n = newn;
	}
}

static void initialize_sort(void)
{
	size_t i;

	for (i = 0; i < cwd_n; i++) {
		sorted[i] = &cwd[i];
	}
}


static size_t register_file(const char *name, const char *dirname, file_type t)
{
	fileinfo_t *f;

	if (cwd_n == cwd_alloc) {
		cwd_alloc = cwd_alloc * 2;
		cwd = realloc(cwd, cwd_alloc * sizeof *cwd);
		if (!cwd) {
			exit(1);
		}
	}
	f = &cwd[cwd_n++];
	f->type = t;
	f->name = xstrdup(name);
	if (flags & TIME || flags & LIST) {
		char absolute[1024];
		file_name_concat(absolute, dirname, name);
		stat(absolute, &f->stat);
		// acl
		// byte size
		// n link
		return f->stat.st_blocks;
	}
	return 0;
}

static void file_name_concat(char *dest, const char *dir, const char *file)
{
	size_t i, j;

	i = 0;
	while (dir[i]) {
		dest[i] = dir[i];
		i++;
	}
	dest[i++] = '/';
	for (j = 0; file[j]; j++) {
		dest[i++] = file[j];
	}
	dest[i] = '\0';
}

static file_type get_type(unsigned char t)
{
	switch (t) {
		case DT_BLK: return blockdev;
		case DT_CHR: return chardev;
		case DT_DIR: return directory;
		case DT_FIFO: return fifo;
		case DT_LNK: return symbolic_link;
		case DT_REG: return normal;
		case DT_SOCK: return sock;
	}
	return unknown;
}

static void clear_files(void)
{
	size_t i;

	for (i = 0; i < cwd_n; i++) {
		fileinfo_t *f = sorted[i];
		free(f->name);
	}
	cwd_n = 0;
}

static void print_dir_name(const char *name);

static int open_pending(int error, pending_t *pending_dir)
{
	char *dir;

	if (!pending_dir) {
		return error;
	}
	dir = pending_dir->name;
	pending = pending_dir->next;
	free(pending_dir);
	print_dir_name(dir);
	error = open_directroy(dir) || error;
	free(dir);
	return open_pending(error, pending);
}

static void print_dir_name(const char *name)
{
	write(1, "\n", 1);
	write(1, name, ft_strlen(name));
	write(1, ":\n", 2);
}

static bool get_opt(const char *opt)
{
	if (!opt || *(opt++) != '-') {
		return false;
	}
	while (*opt) {
		switch (*(opt++)) {
			case 'l':
				flags |= LIST; break;
			case 'R':
				flags |= RECURSIVE; break;
			case 'a':
				flags |= ALL; break;
			case 'r':
				flags |= REVERSE; break;
			case 't':
				flags |= TIME; break;
			default:
				write(2, "ft_ls: illegal option -- ", 26);
				write(2, opt, 1);
				USAGE;
				exit(1);
		}
	}
	return true;
}

static void *xmalloc(size_t size)
{
	void *p;

	if (!(p = malloc(size))) {
		exit(1);
	}
	return p;
}

static char *xstrdup(const char *s)
{
	char *p;

	if (!(p = ft_strdup(s))) {
		exit(1);
	}
	return p;
}
