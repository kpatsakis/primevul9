smtp_report_link_disconnect(struct smtp_session *s)
{
	if (! SESSION_FILTERED(s))
		return;

	report_smtp_link_disconnect("smtp-in", s->id);
}