#include "ft_ls.h"
#include <dirent.h>

int
main(void)
{
	DIR *dir = opendir(".");
	if (!dir)
		return 1;
	struct dirent *rdir;
	while ((rdir = readdir(dir))) {
		ft_printf("%s\n", rdir->d_name);
	}
	closedir(dir);
	return 0;
}
