check_password(char *cp)
{
	unsigned int i;

	if (cp == NULL)
		return PR_FALSE;

	for (i = 0; cp[i] != 0; i++) {
		if (!isprint(cp[i]))
			return PR_FALSE;
	}
	if (i == 0)
		return PR_FALSE;
	return PR_TRUE;
}