expand_proxy_command(const char *proxy_command, const char *user,
    const char *host, int port)
{
	char *tmp, *ret, strport[NI_MAXSERV];

	snprintf(strport, sizeof strport, "%d", port);
	xasprintf(&tmp, "exec %s", proxy_command);
	ret = percent_expand(tmp, "h", host, "p", strport,
	    "r", options.user, (char *)NULL);
	free(tmp);
	return ret;
}