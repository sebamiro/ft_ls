#ifndef FILE_H
#define FILE_H

struct dirent;
typedef struct s_file
{
	struct dirent *rdir;
    char *d_name;
	struct s_file *next;
	struct s_file *previous;
}	t_file;

t_file* new_node(struct dirent *rdir);
t_file* add_lexicographical(t_file **head, struct dirent *rdir);
t_file* last_node(t_file **head);
void free_files(t_file *head);

#endif
