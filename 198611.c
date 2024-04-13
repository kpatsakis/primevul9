static char *getpin(const char *prompt)
{
	char *buf, pass[20];
	int i;

	printf("%s", prompt);
	fflush(stdout);
	if (fgets(pass, 20, stdin) == NULL)
		return NULL;
	for (i = 0; i < 20; i++)
		if (pass[i] == '\n')
			pass[i] = 0;
	if (strlen(pass) == 0)
		return NULL;
	buf = malloc(8);
	if (buf == NULL)
		return NULL;
	if (strlen(pass) > 8) {
		fprintf(stderr, "PIN code too long.\n");
		free(buf);
		return NULL;
	}
	memset(buf, 0, 8);
	strlcpy(buf, pass, 8);
	return buf;
}