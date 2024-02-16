//
// smiro
// printf.c
// 2024-02
//

#include "../io/io.h"

static int hexlen(unsigned int n);

int	puthexnbr(unsigned int n, char x)
{
	char	*nums = "0123456789abcdef";
	int		total = 0;

	if (n >= 16) {
		total += puthexnbr((n / 16), x);
		total += puthexnbr((n % 16), x);
	}
	else {
		if (x == 'X' && n > 9)
			total += ft_putchar_fd(nums[n] - 32, 1);
		else
			total += ft_putchar_fd(nums[n], 1);
	}
	if (total != hexlen(n))
		return (-1);
	return (total);
}

static int hexlen(unsigned int n)
{
	int	x = 1;

	while (n >= 16) {
		x++;
		n = n / 16;
	}
	return (x);
}

