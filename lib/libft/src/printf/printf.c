//
// smiro
// printf.c
// 2024-02
//

#include "../io/io.h"
#include "printf.h"

static int print(const char *s, va_list ap);

int ft_printf(const char *s, ...)
{
	int	 total;
	va_list	ap;

	va_start(ap, s);
	total = print(s, ap);
	va_end(ap);
	return (total);
}

static int findtype(va_list ap, char c, int ident);

static int print(const char *s, va_list ap)
{
	int		temp;
	int		total = 0;
	size_t	i = 0;
	int ident;

	while (s[i]) {
		if (s[i] == '%') {
			ident = identcheck((char *)s + ++i);
			while (s[i] >= '0' && s[i] <= '9') {
				i++;
			}
			if (s[i] == ':') {
				i++;
			}
			temp = findtype(ap, s[i], ident);
		}
		else {
			temp = ft_putchar_fd(s[i], 1);
		}
		if (temp < 0) {
			return (-1);
		}
		total += temp;
		i++;
	}
	return (total);
}

static int findtype(va_list ap, char c, int ident)
{
	if (c == '%')
		return (write(1, "%", 1));
	else if (c == 'c')
		return (handlechar(ap));
	else if (c == 'd' || c == 'i' || c == 'i' || c == 'u'
		|| c == 'x' || c == 'X')
		return (handlenbr(ap, c, ident));
	else if (c == 's')
		return (handlestr(ap, ident));
	else if (c == 'p')
		return (handleptr(ap));
	return (-1);
}

