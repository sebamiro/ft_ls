#include <stdio.h>
#include <dirent.h>

#include "ft_ls.h"
#include "file.h"

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
	while ((rdir = readdir(dir))) {
		add_lexicographical(&file, rdir);
	}
	for (t_file *i = file; i != NULL; i = i->next)
		ft_printf("%s\n", i->rdir->d_name);
	closedir(dir);
	return 0;
}
