//
// smiro
// printf.c
// 2024-02
//

#include "../io/io.h"
#include "printf.h"

static int unlen(unsigned int n);
static int nlen(int n);

int ftputnbr(int n, int ident)
{
	int	total = 0;

	if (n == -2147483648)
		return (write(1, "-2147483648", 11));
	if (ident && ident > 0)
		identprint(ident - nlen(n));
	if (n < 0) {
		total += ft_putchar_fd('-', 1);
		n = -n;
	}
	if (n > 9) {
		total += ftputnbr(n / 10, 0);
		total += ftputnbr(n % 10, 0);
	}
	else
		return (ft_putchar_fd(n + '0', 1));
	if (ident && ident < 0)
		identprint(ident * -1 - nlen(n));
	if (total != nlen(n))
		return (-1);
	return (total);
}

int ftputuint(unsigned int n)
{
	int	total = 0;

	if (n > 9) {
		total += ftputnbr(n / 10, 0);
		total += ftputnbr(n % 10, 0);
	}
	else
		return (ft_putchar_fd(n + '0', 1));
	if (total != unlen(n))
		return (-1);
	return (total);
}

static int unlen(unsigned int n)
{
	int	total = 1;

	while (n > 9) {
		total++;
		n = n / 10;
	}
	return (total);
}

static int nlen(int n)
{
	int	total = 1;

	if (n < 0) {
		total++;
		n = -n;
	}
	while (n > 9) {
		total++;
		n = n / 10;
	}
	return (total);
}

