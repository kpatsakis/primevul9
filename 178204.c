static int fetchve(char ***argv, char ***envp)
{
	char *cmdline = NULL, *environ = NULL;
	size_t cmdline_size, environ_size;

	cmdline = read_file("/proc/self/cmdline", &cmdline_size);
	if (!cmdline)
		goto error;
	environ = read_file("/proc/self/environ", &environ_size);
	if (!environ)
		goto error;

	if (parse_xargs(cmdline, cmdline_size, argv) <= 0)
		goto error;
	if (parse_xargs(environ, environ_size, envp) <= 0)
		goto error;

	return 0;

error:
	free(environ);
	free(cmdline);
	return -EINVAL;
}