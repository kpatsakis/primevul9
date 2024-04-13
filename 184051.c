smtp_report_link_tls(struct smtp_session *s, const char *ssl)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_link_tls("smtp-in", s->id, ssl);
}