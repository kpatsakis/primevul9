static void release_ip(const char *ip, void *priv)
{
	char addr[INET6_ADDRSTRLEN];
	char *p = addr;

	client_socket_addr(get_client_fd(),addr,sizeof(addr));

	if (strncmp("::ffff:", addr, 7) == 0) {
		p = addr + 7;
	}

	if ((strcmp(p, ip) == 0) || ((p != addr) && strcmp(addr, ip) == 0)) {
		/* we can't afford to do a clean exit - that involves
		   database writes, which would potentially mean we
		   are still running after the failover has finished -
		   we have to get rid of this process ID straight
		   away */
		DEBUG(0,("Got release IP message for our IP %s - exiting immediately\n",
			ip));
		/* note we must exit with non-zero status so the unclean handler gets
		   called in the parent, so that the brl database is tickled */
		_exit(1);
	}
}