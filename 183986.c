smtp_session(struct listener *listener, int sock,
    const struct sockaddr_storage *ss, const char *hostname, struct io *io)
{
	struct smtp_session	*s;

	smtp_session_init();

	if ((s = calloc(1, sizeof(*s))) == NULL)
		return (-1);

	s->id = generate_uid();
	s->listener = listener;
	memmove(&s->ss, ss, sizeof(*ss));

	if (io != NULL)
		s->io = io;
	else
		s->io = io_new();

	io_set_callback(s->io, smtp_io, s);
	io_set_fd(s->io, sock);
	io_set_timeout(s->io, SMTPD_SESSION_TIMEOUT * 1000);
	io_set_write(s->io);
	s->state = STATE_NEW;

	(void)strlcpy(s->smtpname, listener->hostname, sizeof(s->smtpname));

	log_trace(TRACE_SMTP, "smtp: %p: connected to listener %p "
	    "[hostname=%s, port=%d, tag=%s]", s, listener,
	    listener->hostname, ntohs(listener->port), listener->tag);

	/* For local enqueueing, the hostname is already set */
	if (hostname) {
		s->flags |= SF_AUTHENTICATED;
		/* A bit of a hack */
		if (!strcmp(hostname, "localhost"))
			s->flags |= SF_BOUNCE;
		(void)strlcpy(s->rdns, hostname, sizeof(s->rdns));
		s->fcrdns = 1;
		smtp_lookup_servername(s);
	} else {
		resolver_getnameinfo((struct sockaddr *)&s->ss, NI_NAMEREQD,
		    smtp_getnameinfo_cb, s);
	}

	/* session may have been freed by now */

	return (0);
}