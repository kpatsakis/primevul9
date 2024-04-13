smtp_proceed_ehlo(struct smtp_session *s, const char *args)
{
	(void)strlcpy(s->helo, args, sizeof(s->helo));
	s->flags &= SF_SECURE | SF_AUTHENTICATED | SF_VERIFIED;
	s->flags |= SF_EHLO;
	s->flags |= SF_8BITMIME;

	smtp_report_link_identify(s, "EHLO", s->helo);

	smtp_enter_state(s, STATE_HELO);
	smtp_reply(s, "250-%s Hello %s %s%s%s, pleased to meet you",
	    s->smtpname,
	    s->helo,
	    s->ss.ss_family == AF_INET6 ? "" : "[",
	    ss_to_text(&s->ss),
	    s->ss.ss_family == AF_INET6 ? "" : "]");

	smtp_reply(s, "250-8BITMIME");
	smtp_reply(s, "250-ENHANCEDSTATUSCODES");
	smtp_reply(s, "250-SIZE %zu", env->sc_maxsize);
	if (ADVERTISE_EXT_DSN(s))
		smtp_reply(s, "250-DSN");
	if (ADVERTISE_TLS(s))
		smtp_reply(s, "250-STARTTLS");
	if (ADVERTISE_AUTH(s))
		smtp_reply(s, "250-AUTH PLAIN LOGIN");
	smtp_reply(s, "250 HELP");
}