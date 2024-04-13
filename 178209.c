static int initpipe(void)
{
	int pipenum;
	char *initpipe, *endptr;

	initpipe = getenv("_LIBCONTAINER_INITPIPE");
	if (initpipe == NULL || *initpipe == '\0')
		return -1;

	pipenum = strtol(initpipe, &endptr, 10);
	if (*endptr != '\0')
		bail("unable to parse _LIBCONTAINER_INITPIPE");

	return pipenum;
}