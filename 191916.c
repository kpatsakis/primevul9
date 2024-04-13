strdigits_len(char *str)
{
	char	   *p = str;
	int			len;

	len = strspace_len(str);
	p += len;

	while (*p && isdigit((unsigned char) *p) && len <= DCH_MAX_ITEM_SIZ)
	{
		len++;
		p++;
	}
	return len;
}