static int git_tcp_connect_sock(char *host, int flags)
{
	struct strbuf error_message = STRBUF_INIT;
	int sockfd = -1;
	const char *port = STR(DEFAULT_GIT_PORT);
	char *ep;
	struct hostent *he;
	struct sockaddr_in sa;
	char **ap;
	unsigned int nport;
	int cnt;

	get_host_and_port(&host, &port);

	if (flags & CONNECT_VERBOSE)
		fprintf(stderr, _("Looking up %s ... "), host);

	he = gethostbyname(host);
	if (!he)
		die(_("unable to look up %s (%s)"), host, hstrerror(h_errno));
	nport = strtoul(port, &ep, 10);
	if ( ep == port || *ep ) {
		/* Not numeric */
		struct servent *se = getservbyname(port,"tcp");
		if ( !se )
			die(_("unknown port %s"), port);
		nport = se->s_port;
	}

	if (flags & CONNECT_VERBOSE)
		/* TRANSLATORS: this is the end of "Looking up %s ... " */
		fprintf(stderr, _("done.\nConnecting to %s (port %s) ... "), host, port);

	for (cnt = 0, ap = he->h_addr_list; *ap; ap++, cnt++) {
		memset(&sa, 0, sizeof sa);
		sa.sin_family = he->h_addrtype;
		sa.sin_port = htons(nport);
		memcpy(&sa.sin_addr, *ap, he->h_length);

		sockfd = socket(he->h_addrtype, SOCK_STREAM, 0);
		if ((sockfd < 0) ||
		    connect(sockfd, (struct sockaddr *)&sa, sizeof sa) < 0) {
			strbuf_addf(&error_message, "%s[%d: %s]: errno=%s\n",
				host,
				cnt,
				inet_ntoa(*(struct in_addr *)&sa.sin_addr),
				strerror(errno));
			if (0 <= sockfd)
				close(sockfd);
			sockfd = -1;
			continue;
		}
		if (flags & CONNECT_VERBOSE)
			fprintf(stderr, "%s ",
				inet_ntoa(*(struct in_addr *)&sa.sin_addr));
		break;
	}

	if (sockfd < 0)
		die(_("unable to connect to %s:\n%s"), host, error_message.buf);

	enable_keepalive(sockfd);

	if (flags & CONNECT_VERBOSE)
		/* TRANSLATORS: this is the end of "Connecting to %s (port %s) ... " */
		fprintf_ln(stderr, _("done."));

	return sockfd;
}