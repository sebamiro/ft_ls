//
// smiro
// printf.h
// 2024-02
//

#ifndef PRINTF_H
# define PRINTF_H

#include <stdarg.h>
#include <unistd.h>


int ft_printf(const char *s, ...);
int identcheck(char *str);
int identprint(int n);
int handlechar(va_list ap);
int handlenbr(va_list ap, char c,  int ident);
int handlestr(va_list ap, int ident);
int handleptr(va_list ap);
int	puthexnbr(unsigned int n, char x);
int ftputnbr(int n, int ident);
int ftputuint(unsigned int n);
int putptr(unsigned long long ptr);


#endif

