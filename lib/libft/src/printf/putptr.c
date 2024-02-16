//
// smiro
// printf.h
// 2024-02
//

#include <unistd.h>

#include "../io/io.h"

static int ptrlen(uintptr_t ptr);
static int printptr(uintptr_t ptr);

int putptr(unsigned long long ptr)
{
	int	total  = 0;

	if (ptr == 0)
		return (write(1, "(nil)", 5));
	total += write(1, "0x", 2);
	total += printptr(ptr);
	if (total != ptrlen(ptr) + 2)
		return (-1);
	return (total);
}

static int ptrlen(uintptr_t ptr)
{
	int	total = 1;

	while (ptr >= 16) {
		ptr = ptr / 16;
		total++;
	}
	return (total);
}

static int printptr(uintptr_t ptr)
{
	int	total  = 0;

	if (ptr >= 16) {
		total += printptr(ptr / 16);
		total += printptr(ptr % 16);
	}
	else if (ptr < 10)
		total += ft_putchar_fd((ptr + '0'), 1);
	else
		total += ft_putchar_fd(ptr - 10 + 'a', 1);
	return (total);
}

