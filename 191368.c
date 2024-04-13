ssh_local_cmd(const char *args)
{
	char *shell;
	pid_t pid;
	int status;
	void (*osighand)(int);

	if (!options.permit_local_command ||
	    args == NULL || !*args)
		return (1);

	if ((shell = getenv("SHELL")) == NULL || *shell == '\0')
		shell = _PATH_BSHELL;

	osighand = signal(SIGCHLD, SIG_DFL);
	pid = fork();
	if (pid == 0) {
		signal(SIGPIPE, SIG_DFL);
		debug3("Executing %s -c \"%s\"", shell, args);
		execl(shell, shell, "-c", args, (char *)NULL);
		error("Couldn't execute %s -c \"%s\": %s",
		    shell, args, strerror(errno));
		_exit(1);
	} else if (pid == -1)
		fatal("fork failed: %.100s", strerror(errno));
	while (waitpid(pid, &status, 0) == -1)
		if (errno != EINTR)
			fatal("Couldn't wait for child: %s", strerror(errno));
	signal(SIGCHLD, osighand);

	if (!WIFEXITED(status))
		return (1);

	return (WEXITSTATUS(status));
}