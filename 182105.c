static struct child_process *git_connect_git(int fd[2], char *hostandport,
					     const char *path, const char *prog,
					     enum protocol_version version,
					     int flags)
{
	struct child_process *conn;
	struct strbuf request = STRBUF_INIT;
	/*
	 * Set up virtual host information based on where we will
	 * connect, unless the user has overridden us in
	 * the environment.
	 */
	char *target_host = getenv("GIT_OVERRIDE_VIRTUAL_HOST");
	if (target_host)
		target_host = xstrdup(target_host);
	else
		target_host = xstrdup(hostandport);

	transport_check_allowed("git");
	if (strchr(target_host, '\n') || strchr(path, '\n'))
		die(_("newline is forbidden in git:// hosts and repo paths"));

	/*
	 * These underlying connection commands die() if they
	 * cannot connect.
	 */
	if (git_use_proxy(hostandport))
		conn = git_proxy_connect(fd, hostandport);
	else
		conn = git_tcp_connect(fd, hostandport, flags);
	/*
	 * Separate original protocol components prog and path
	 * from extended host header with a NUL byte.
	 *
	 * Note: Do not add any other headers here!  Doing so
	 * will cause older git-daemon servers to crash.
	 */
	strbuf_addf(&request,
		    "%s %s%chost=%s%c",
		    prog, path, 0,
		    target_host, 0);

	/* If using a new version put that stuff here after a second null byte */
	if (version > 0) {
		strbuf_addch(&request, '\0');
		strbuf_addf(&request, "version=%d%c",
			    version, '\0');
	}

	packet_write(fd[1], request.buf, request.len);

	free(target_host);
	strbuf_release(&request);
	return conn;
}