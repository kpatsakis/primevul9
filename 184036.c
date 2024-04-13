smtp_report_link_greeting(struct smtp_session *s,
    const char *domain)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_link_greeting("smtp-in", s->id, domain);
}