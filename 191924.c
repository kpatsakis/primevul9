fill_str(char *str, int c, int max)
{
	memset(str, c, max);
	*(str + max) = '\0';
	return str;
}