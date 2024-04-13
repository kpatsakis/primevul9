asc_toupper(const char *buff, size_t nbytes)
{
	char	   *result;
	char	   *p;

	if (!buff)
		return NULL;

	result = pnstrdup(buff, nbytes);

	for (p = result; *p; p++)
		*p = pg_ascii_toupper((unsigned char) *p);

	return result;
}