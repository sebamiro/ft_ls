////////////////////////////////////////////////////////////////////////////////
// _struct_utils.c
////////////////////////////////////////////////////////////////////////////////

#include <dirent.h>

#include "ft_ls.h"
#include "file.h"

t_file*
last_node(t_file **head)
{
	t_file *current = *head;

	while (current->next)
		current = current->next;
	return current;
}

t_file*
new_node(struct dirent *rdir)
{
	t_file *new = ft_calloc(1, sizeof(t_file));
	if (!new)
		return NULL;
	new->rdir = rdir;
	return new;
}

t_file*
add_lexicographical(t_file **head, struct dirent *rdir)
{
	t_file *temp = *head;
	t_file *new = new_node(rdir);
	if (!new)
		return NULL;
	if (!temp) {
		*head = new;
		return new;
	}
	while (temp) {
		if (ft_strncmp(temp->rdir->d_name, rdir->d_name, ft_strlen(rdir->d_name)) > 0) {
			new->next = temp;
			new->previous = temp->previous;
			if (new->previous) {
				new->previous->next = new;
			}
			else {
				*head = new;
			}
			temp->previous = new;
			return new;
		}
		temp = temp->next;
	}
	t_file *last = last_node(head);
	last->next = new;
	new->previous = last;
	return new;
}
