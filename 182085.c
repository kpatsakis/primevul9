static struct child_process *git_proxy_connect(int fd[2], char *host)
{
	const char *port = STR(DEFAULT_GIT_PORT);
	struct child_process *proxy;

	get_host_and_port(&host, &port);

	if (looks_like_command_line_option(host))
		die(_("strange hostname '%s' blocked"), host);
	if (looks_like_command_line_option(port))
		die(_("strange port '%s' blocked"), port);

	proxy = xmalloc(sizeof(*proxy));
	child_process_init(proxy);
	argv_array_push(&proxy->args, git_proxy_command);
	argv_array_push(&proxy->args, host);
	argv_array_push(&proxy->args, port);
	proxy->in = -1;
	proxy->out = -1;
	if (start_command(proxy))
		die(_("cannot start proxy %s"), git_proxy_command);
	fd[0] = proxy->out; /* read from proxy stdout */
	fd[1] = proxy->in;  /* write to proxy stdin */
	return proxy;
}