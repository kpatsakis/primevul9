asc_initcap(const char *buff, size_t nbytes)
{
	char	   *result;
	char	   *p;
	int			wasalnum = false;

	if (!buff)
		return NULL;

	result = pnstrdup(buff, nbytes);

	for (p = result; *p; p++)
	{
		char		c;

		if (wasalnum)
			*p = c = pg_ascii_tolower((unsigned char) *p);
		else
			*p = c = pg_ascii_toupper((unsigned char) *p);
		/* we don't trust isalnum() here */
		wasalnum = ((c >= 'A' && c <= 'Z') ||
					(c >= 'a' && c <= 'z') ||
					(c >= '0' && c <= '9'));
	}

	return result;
}