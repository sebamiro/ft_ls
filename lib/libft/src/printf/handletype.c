//
// smiro
// printf.c
// 2024-02
//

#include "../string/string.h"
#include "../io/io.h"
#include "printf.h"

int handlechar(va_list ap)
{
	char c = (char)va_arg(ap, int);
	return (write(1, &c, 1));
}

int handlenbr(va_list ap, char c,  int ident)
{
	if (c == 'd' || c == 'i')
		return (ftputnbr(va_arg(ap, int), ident));
	else if (c == 'u')
		return (ftputuint(va_arg(ap, unsigned int)));
	else if (c == 'x' || c == 'X')
		return (puthexnbr(va_arg(ap, unsigned int), c));
	return (-1);
}

int handlestr(va_list ap, int ident)
{
	char	*str;
	int		total = 0;
	size_t	i = 0;

	str = va_arg(ap, char *);
	if (!str)
		return (write(1, "(null)", 6));
	if (ident && ident > 0)
		identprint(ident - ft_strlen(str));
	while (str[i]) {
		if (ft_putchar_fd(str[i], 1) < 0)
			return (-1);
		total += 1;
		i++;
	}
	if (ident && ident < 0)
		identprint(ident * -1 - ft_strlen(str));
	return (total);
}

int handleptr(va_list ap)
{
	unsigned long long	n;

	n = va_arg(ap, unsigned long long);
	return (putptr(n));
}
