smtp_getaddrinfo_cb(void *arg, int gaierrno, struct addrinfo *ai0)
{
	struct smtp_session *s = arg;
	struct addrinfo *ai;
	char fwd[64], rev[64];

	if (gaierrno) {
		if (gaierrno == EAI_NODATA || gaierrno == EAI_NONAME)
			s->fcrdns = 0;
		else {
			log_warnx("getaddrinfo: %s: %s", s->rdns,
			    gai_strerror(gaierrno));
			s->fcrdns = -1;
		}
	}
	else {
		strlcpy(rev, ss_to_text(&s->ss), sizeof(rev));
		for (ai = ai0; ai; ai = ai->ai_next) {
			strlcpy(fwd, sa_to_text(ai->ai_addr), sizeof(fwd));
			if (!strcmp(fwd, rev)) {
				s->fcrdns = 1;
				break;
			}
		}
		freeaddrinfo(ai0);
	}

	smtp_lookup_servername(s);
}