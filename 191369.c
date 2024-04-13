ssh_proxy_fdpass_connect(const char *host, u_short port,
    const char *proxy_command)
{
	char *command_string;
	int sp[2], sock;
	pid_t pid;
	char *shell;

	if ((shell = getenv("SHELL")) == NULL)
		shell = _PATH_BSHELL;

	if (socketpair(AF_UNIX, SOCK_STREAM, 0, sp) < 0)
		fatal("Could not create socketpair to communicate with "
		    "proxy dialer: %.100s", strerror(errno));

	command_string = expand_proxy_command(proxy_command, options.user,
	    host, port);
	debug("Executing proxy dialer command: %.500s", command_string);

	/* Fork and execute the proxy command. */
	if ((pid = fork()) == 0) {
		char *argv[10];

		/* Child.  Permanently give up superuser privileges. */
		permanently_drop_suid(original_real_uid);

		close(sp[1]);
		/* Redirect stdin and stdout. */
		if (sp[0] != 0) {
			if (dup2(sp[0], 0) < 0)
				perror("dup2 stdin");
		}
		if (sp[0] != 1) {
			if (dup2(sp[0], 1) < 0)
				perror("dup2 stdout");
		}
		if (sp[0] >= 2)
			close(sp[0]);

		/*
		 * Stderr is left as it is so that error messages get
		 * printed on the user's terminal.
		 */
		argv[0] = shell;
		argv[1] = "-c";
		argv[2] = command_string;
		argv[3] = NULL;

		/*
		 * Execute the proxy command.
		 * Note that we gave up any extra privileges above.
		 */
		execv(argv[0], argv);
		perror(argv[0]);
		exit(1);
	}
	/* Parent. */
	if (pid < 0)
		fatal("fork failed: %.100s", strerror(errno));
	close(sp[0]);
	free(command_string);

	if ((sock = mm_receive_fd(sp[1])) == -1)
		fatal("proxy dialer did not pass back a connection");

	while (waitpid(pid, NULL, 0) == -1)
		if (errno != EINTR)
			fatal("Couldn't wait for child: %s", strerror(errno));

	/* Set the connection file descriptors. */
	packet_set_connection(sock, sock);

	return 0;
}