smtp_getnameinfo_cb(void *arg, int gaierrno, const char *host, const char *serv)
{
	struct smtp_session *s = arg;
	struct addrinfo hints;

	if (gaierrno) {
		(void)strlcpy(s->rdns, "<unknown>", sizeof(s->rdns));

		if (gaierrno == EAI_NODATA || gaierrno == EAI_NONAME)
			s->fcrdns = 0;
		else {
			log_warnx("getnameinfo: %s: %s", ss_to_text(&s->ss),
			    gai_strerror(gaierrno));
			s->fcrdns = -1;
		}

		smtp_lookup_servername(s);
		return;
	}

	(void)strlcpy(s->rdns, host, sizeof(s->rdns));

	memset(&hints, 0, sizeof(hints));
	hints.ai_family = s->ss.ss_family;
	hints.ai_socktype = SOCK_STREAM;
	resolver_getaddrinfo(s->rdns, NULL, &hints, smtp_getaddrinfo_cb, s);
}