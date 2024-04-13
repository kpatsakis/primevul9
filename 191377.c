ssh_kill_proxy_command(void)
{
	/*
	 * Send SIGHUP to proxy command if used. We don't wait() in
	 * case it hangs and instead rely on init to reap the child
	 */
	if (proxy_command_pid > 1)
		kill(proxy_command_pid, SIGHUP);
}