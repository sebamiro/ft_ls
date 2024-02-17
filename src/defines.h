//
// smiro ft_ls
// defines.h
// 2024-02
//

#ifndef DEFINES_H
# define DEFINES_H

# include <sys/stat.h>
# include <stdlib.h>

# define USAGE write(2, "usage: ft_ls [-lRartG]\n", 22);

# define RESET "\e[0m"
# define RED   "\e[31m"
# define GREEN "\e[32m"
# define BLUE  "\e[34m"
# define PURPLE "\e[35m"

enum flags {
	LIST =		0x01,
	RECURSIVE = 0x02,
	ALL	=		0x04,
	REVERSE =	0x08,
	TIME =		0x10,
	COLORS =	0x20,
	NO_SORT =	0x40,
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


#endif
