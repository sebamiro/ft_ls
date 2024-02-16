//
// smiro ft_ls
// defines.h
// 2024-02
//

#ifndef DEFINES_H
# define DEFINES_H

# include <sys/stat.h>
# include <stdlib.h>

# define USAGE write(2, "usage: ft_ls [-lRart]\n", 22);

enum flags {
	LIST =		0x01,
	RECURSIVE = 0x02,
	ALL	=		0x04,
	REVERSE =	0x08,
	TIME =		0x10,
};

typedef enum e_file_type {
    unknown,
    blockdev,
    chardev,
    directory,
    fifo,
    symbolic_link,
    normal,
    sock
} file_type;

typedef struct s_pending
{
    char				*name;
    struct s_pending	*next;
} pending_t;

typedef struct s_fileinfo
{
    char		*name;
    file_type	type;
    struct stat stat;
    char		acl;
    char		*link;
} fileinfo_t;

typedef char flag_t;

flag_t flags = 0;

pending_t	*pending = NULL;

fileinfo_t	*cwd;
size_t		cwd_n;
size_t		cwd_alloc;

void		**sorted = NULL;
size_t		sorted_alloc = 0;

#endif
