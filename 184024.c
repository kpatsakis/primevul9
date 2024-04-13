smtp_proceed_helo(struct smtp_session *s, const char *args)
{
	(void)strlcpy(s->helo, args, sizeof(s->helo));
	s->flags &= SF_SECURE | SF_AUTHENTICATED | SF_VERIFIED;

	smtp_report_link_identify(s, "HELO", s->helo);

	smtp_enter_state(s, STATE_HELO);

	smtp_reply(s, "250 %s Hello %s %s%s%s, pleased to meet you",
	    s->smtpname,
	    s->helo,
	    s->ss.ss_family == AF_INET6 ? "" : "[",
	    ss_to_text(&s->ss),
	    s->ss.ss_family == AF_INET6 ? "" : "]");
}