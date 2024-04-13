static int php_check_dots(const char *element, int n)
{
	while (n-- > 0) if (element[n] != '.') break;

	return (n != -1);
}