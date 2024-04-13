strspace_len(char *str)
{
	int			len = 0;

	while (*str && isspace((unsigned char) *str))
	{
		str++;
		len++;
	}
	return len;
}