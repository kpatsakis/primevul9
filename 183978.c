smtp_report_timeout(struct smtp_session *s)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_timeout("smtp-in", s->id);
}