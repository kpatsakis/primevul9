isdn_star(char *s, char *p)
{
	while (isdn_wildmat(s, p)) {
		if (*++s == '\0')
			return (2);
	}
	return (0);
}